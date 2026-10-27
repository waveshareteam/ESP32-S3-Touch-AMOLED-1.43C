#ifndef ESP32_S3_TOUCH_AMOLED_1_43C_H
#define ESP32_S3_TOUCH_AMOLED_1_43C_H

#include <driver/i2c_master.h>
#include "lvgl.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  lv_indev_t *indev;
  lv_disp_t *disp;
} bsp_lvgl_t;

void bsp_lcd_init(void);
void bsp_touch_init(i2c_master_bus_handle_t i2c_bus_);
esp_err_t bsp_display_brightness_init(void);
esp_err_t bsp_display_brightness_set(int brightness_percent);
int bsp_display_brightness_get(void);
esp_err_t bsp_display_lock(int32_t timeout_ms);
void bsp_display_unlock(void);
bsp_lvgl_t bsp_broolesia_display_init(void);
bsp_lvgl_t bsp_get_broolesia_display(void);
i2c_master_bus_handle_t bsp_i2c_get_handle(void);
void bsp_batt_init(void);
uint16_t bsp_batt_get_voltage(void);
uint8_t bsp_batt_get_status(void);
esp_err_t bsp_spiffs_mount(void);

#ifdef __cplusplus
}
#endif


#endif