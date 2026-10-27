#pragma once
#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    esp_err_t i2c, display, touch;
    esp_err_t button, audio, wifi;
} bsp_init_status_t;

/* Call once from app_main before starting APPs. Required display errors are
 * returned; optional peripheral failures are logged and stored in status. */
esp_err_t bsp_init(void);
const bsp_init_status_t *bsp_get_init_status(void);

lv_display_t *bsp_display_get(void);
esp_lcd_touch_handle_t bsp_touch_get_handle(void);
esp_err_t bsp_user_button_init(void);
uint32_t bsp_user_button_get_press_count(void);

typedef enum {
    BSP_BUTTON_ID_BOOT,
} bsp_button_id_t;
/* Normalized physical state: BOOT GPIO0 is active-low. */
esp_err_t bsp_button_get_pressed(bsp_button_id_t button, bool *pressed);

esp_err_t bsp_wifi_init(void);
/* Start the initialized radio for scanning/provisioning. Does not connect. */
esp_err_t bsp_wifi_start(void);

#ifdef __cplusplus
}
#endif
