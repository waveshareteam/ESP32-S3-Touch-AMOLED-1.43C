#ifndef ESP32_S3_TOUCH_AMOLED_1_43C_H
#define ESP32_S3_TOUCH_AMOLED_1_43C_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void bsp_batt_init(void);
uint16_t bsp_batt_get_voltage(void);
uint8_t bsp_batt_get_status(void);

#ifdef __cplusplus
}
#endif


#endif