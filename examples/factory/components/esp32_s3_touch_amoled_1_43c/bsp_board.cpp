#include "bsp/esp-bsp.h"
#include <atomic>
#include "esp_check.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "bsp_board_extra.h"
#include "bsp_board_internal.h"
#include "iot_button.h"
#include "button_gpio.h"

static const char *TAG = "bsp_1_43c";
static bool wifi_ready;
static button_handle_t boot_button;
static std::atomic<uint32_t> button_presses{0};
bsp_init_status_t bsp_board_status = {
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE,
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE
};

bool bsp_board_i2c_in_use(void) { return bsp_board_audio_in_use(); }
const bsp_init_status_t *bsp_get_init_status(void) { return &bsp_board_status; }

static void button_pressed(void *, void *) { button_presses.fetch_add(1); }
esp_err_t bsp_user_button_init(void)
{
    if (boot_button) return ESP_OK;
    button_config_t cfg = {};
    button_gpio_config_t gpio = {};
    gpio.gpio_num = BSP_BUTTON_BOOT;
    gpio.active_level = 0;
    ESP_RETURN_ON_ERROR(iot_button_new_gpio_device(&cfg, &gpio, &boot_button), TAG, "BOOT button");
    esp_err_t ret = iot_button_register_cb(boot_button, BUTTON_PRESS_DOWN, nullptr, button_pressed, nullptr);
    if (ret != ESP_OK) { iot_button_delete(boot_button); boot_button = nullptr; }
    return ret;
}

uint32_t bsp_user_button_get_press_count(void) { return button_presses.load(); }

esp_err_t bsp_button_get_pressed(bsp_button_id_t button, bool *pressed)
{
    if (!pressed) return ESP_ERR_INVALID_ARG;
    switch (button) {
    case BSP_BUTTON_ID_BOOT:
        if (!boot_button) return ESP_ERR_INVALID_STATE;
        *pressed = gpio_get_level(BSP_BUTTON_BOOT) == 0;
        return ESP_OK;
    default:
        return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t bsp_wifi_init(void)
{
    if (wifi_ready) return ESP_OK;
    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "Netif");
    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) return ret;
    static esp_netif_t *sta, *ap;
    if (!sta) sta = esp_netif_create_default_wifi_sta();
    if (!ap) ap = esp_netif_create_default_wifi_ap();
    if (!sta || !ap) return ESP_ERR_NO_MEM;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "Wi-Fi driver");
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) { esp_wifi_deinit(); return ret; }
    // Connection credentials, scanning and provisioning belong to network_service.
    wifi_ready = true;
    return ESP_OK;
}

static void report(const char *name, esp_err_t result)
{
    if (result == ESP_OK) ESP_LOGI(TAG, "%s ready", name);
    else ESP_LOGW(TAG, "%s unavailable: %s", name, esp_err_to_name(result));
}

esp_err_t bsp_wifi_start(void)
{
    if (!wifi_ready) return ESP_ERR_INVALID_STATE;
    // esp_wifi_start is idempotent when the initialized Wi-Fi driver is running.
    return esp_wifi_start();
}

esp_err_t bsp_init(void)
{
    // Hold the backlight off from the start of board initialization.
    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Backlight initialization");
    esp_err_t ret = nvs_flash_init();
    // Preserve saved data on NVS incompatibility; report instead of silently erasing.
    ESP_RETURN_ON_ERROR(ret, TAG, "NVS initialization");
    if (!bsp_display_start()) {
        if (bsp_board_status.i2c != ESP_OK) return bsp_board_status.i2c;
        return bsp_board_status.display;
    }
    bsp_board_status.button = bsp_user_button_init(); report("BOOT button", bsp_board_status.button);
    bsp_board_status.audio = bsp_extra_codec_init(); report("ES8311 + ES7210", bsp_board_status.audio);
    bsp_board_status.wifi = bsp_wifi_init(); report("Wi-Fi driver", bsp_board_status.wifi);
    return ESP_OK;
}
