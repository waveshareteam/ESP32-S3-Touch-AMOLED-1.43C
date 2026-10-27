#pragma once
#include "esp_lcd_touch.h"

/* CST820: one touch point, 7-bit I2C address 0x15. */
esp_err_t bsp_touch_new_cst820(esp_lcd_panel_io_handle_t io,
                              const esp_lcd_touch_config_t *config,
                              esp_lcd_touch_handle_t *touch);
