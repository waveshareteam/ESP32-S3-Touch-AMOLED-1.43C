#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <esp_log.h>
#include "esp32_s3_touch_amoled_1.43c.h"

#define TAG   "main"

void Batt_LoopTask(void *arg) {
    while(1) {
        ESP_LOGW(TAG,"Voltage:%dmV,Charging-Status:%s",bsp_batt_get_voltage(),bsp_batt_get_status() ? "Charging" : "No Charging");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

extern "C" void app_main(void) {
    ESP_LOGW(TAG,"adc-example run");
    bsp_batt_init();
    xTaskCreate(Batt_LoopTask, "Batt_LoopTask", 4 * 1024, NULL, 3, NULL);
}


