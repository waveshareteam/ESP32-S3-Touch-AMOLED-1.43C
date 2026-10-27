/* SPDX-License-Identifier: Apache-2.0 */
#include "esp_lcd_touch_cst820.h"
#include <cstdlib>
#include <cstring>
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "CST820";

static esp_err_t read_data(esp_lcd_touch_handle_t touch)
{
    uint8_t data[5] = {};
    // 0x02: point count; 0x03..0x06: 12-bit X/Y, high nibbles carry events.
    esp_err_t ret = esp_lcd_panel_io_rx_param(touch->io, 0x02, data, sizeof(data));
    portENTER_CRITICAL(&touch->data.lock);
    touch->data.points = ret == ESP_OK && (data[0] & 0x0f) == 1 ? 1 : 0;
    if (touch->data.points) {
        touch->data.coords[0].x = ((data[1] & 0x0f) << 8) | data[2];
        touch->data.coords[0].y = ((data[3] & 0x0f) << 8) | data[4];
        touch->data.coords[0].strength = 1;
    }
    portEXIT_CRITICAL(&touch->data.lock);
    return ret;
}

static bool get_coordinates(esp_lcd_touch_handle_t touch, uint16_t *x, uint16_t *y,
                            uint16_t *strength, uint8_t *count, uint8_t max_count)
{
    if (!x || !y || !count || !max_count) return false;
    portENTER_CRITICAL(&touch->data.lock);
    *count = touch->data.points ? 1 : 0;
    if (*count) {
        x[0] = touch->data.coords[0].x;
        y[0] = touch->data.coords[0].y;
        if (strength) strength[0] = touch->data.coords[0].strength;
    }
    touch->data.points = 0;
    portEXIT_CRITICAL(&touch->data.lock);
    return *count != 0;
}

static esp_err_t delete_touch(esp_lcd_touch_handle_t touch)
{
    if (touch->config.int_gpio_num != GPIO_NUM_NC) {
        if (touch->config.interrupt_callback) gpio_isr_handler_remove(touch->config.int_gpio_num);
        gpio_reset_pin(touch->config.int_gpio_num);
    }
    if (touch->config.rst_gpio_num != GPIO_NUM_NC) gpio_reset_pin(touch->config.rst_gpio_num);
    heap_caps_free(touch);
    return ESP_OK;
}

esp_err_t bsp_touch_new_cst820(esp_lcd_panel_io_handle_t io,
                              const esp_lcd_touch_config_t *config,
                              esp_lcd_touch_handle_t *result)
{
    if (!io || !config || !result) return ESP_ERR_INVALID_ARG;
    *result = nullptr;
    auto *touch = static_cast<esp_lcd_touch_t *>(heap_caps_calloc(
        1, sizeof(esp_lcd_touch_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    if (!touch) return ESP_ERR_NO_MEM;
    touch->io = io; touch->config = *config;
    touch->data.lock = portMUX_INITIALIZER_UNLOCKED;
    touch->read_data = read_data; touch->get_xy = get_coordinates; touch->del = delete_touch;
    auto initialize = [&]() -> esp_err_t {
        if (config->rst_gpio_num != GPIO_NUM_NC) {
            gpio_config_t pin = {};
            pin.pin_bit_mask = 1ULL << config->rst_gpio_num;
            pin.mode = GPIO_MODE_OUTPUT; pin.pull_up_en = GPIO_PULLUP_ENABLE;
            ESP_RETURN_ON_ERROR(gpio_config(&pin), TAG, "Reset GPIO");
            // Keep the module's validated high/low/high power-up timing.
            ESP_RETURN_ON_ERROR(gpio_set_level(config->rst_gpio_num, !config->levels.reset), TAG, "Reset inactive");
            vTaskDelay(pdMS_TO_TICKS(200));
            ESP_RETURN_ON_ERROR(gpio_set_level(config->rst_gpio_num, config->levels.reset), TAG, "Reset active");
            vTaskDelay(pdMS_TO_TICKS(200));
            ESP_RETURN_ON_ERROR(gpio_set_level(config->rst_gpio_num, !config->levels.reset), TAG, "Reset released");
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        if (config->int_gpio_num != GPIO_NUM_NC) {
            gpio_config_t pin = {};
            pin.pin_bit_mask = 1ULL << config->int_gpio_num; pin.mode = GPIO_MODE_INPUT;
            pin.intr_type = config->levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE;
            ESP_RETURN_ON_ERROR(gpio_config(&pin), TAG, "Interrupt GPIO");
            if (config->interrupt_callback)
                ESP_RETURN_ON_ERROR(esp_lcd_touch_register_interrupt_callback(touch, config->interrupt_callback), TAG, "Interrupt callback");
        }
        // Verify the same report register used by normal reads; no foreign chip-ID gate.
        return read_data(touch);
    };
    esp_err_t ret = initialize();
    if (ret != ESP_OK) { delete_touch(touch); return ret; }
    touch->data.points = 0;
    *result = touch;
    return ESP_OK;
}
