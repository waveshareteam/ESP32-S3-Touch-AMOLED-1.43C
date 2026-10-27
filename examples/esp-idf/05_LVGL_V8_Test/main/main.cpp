#include <stdio.h>
#include <Freertos/FreeRTOS.h>
#include <esp_log.h>
#include "esp32_s3_touch_amoled_1.43c.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "bsp_config.h"

#define TAG   "main"

#define BACKLIGHT_EN 1


#if (BACKLIGHT_EN)
void Backlight_LoopTask(void *arg) {
    int8_t brightness = 100;
    while(1) {
        if(brightness < 0) {
            brightness = 100;
        }
        if(bsp_display_lock(-1) == ESP_OK) {
            bsp_display_brightness_set(brightness);
            bsp_display_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        brightness -= 25; 
    }
}
#endif

extern "C" void app_main(void) {
    bsp_broolesia_display_init();
    if(bsp_display_lock(-1) == ESP_OK) {
        lv_demo_widgets();
        bsp_display_unlock();
    }
#if (BACKLIGHT_EN)
    xTaskCreatePinnedToCore(Backlight_LoopTask, "Backlight_LoopTask", 4 * 1024, NULL, 2, NULL,0);
#endif
}


