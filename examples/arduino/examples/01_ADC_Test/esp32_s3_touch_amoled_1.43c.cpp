#include "esp32_s3_touch_amoled_1.43c.h"
#include "bsp_config.h"

#include <esp_adc/adc_oneshot.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>

#define TAG "DISP_BSP"

static adc_cali_handle_t cali_handle;
static adc_oneshot_unit_handle_t adc1_handle;

void bsp_batt_init(void) {
  adc_cali_curve_fitting_config_t cali_config = {};
  cali_config.unit_id = ADC_UNIT_1;
  cali_config.atten = ADC_ATTEN_DB_12;
  cali_config.bitwidth = ADC_BITWIDTH_12;
  ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));

  adc_oneshot_unit_init_cfg_t init_config1 = {};
  init_config1.unit_id = ADC_UNIT_1;
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  adc_oneshot_chan_cfg_t config = {};
  config.bitwidth = ADC_BITWIDTH_12;
  config.atten = ADC_ATTEN_DB_12;
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3, &config));

  gpio_config_t gpio_conf = {};
  gpio_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_conf.mode = GPIO_MODE_INPUT;
  gpio_conf.pin_bit_mask = (0x1ULL << GPIO_NUM_7);
  gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;

  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&gpio_conf));
}

uint16_t bsp_batt_get_voltage(void) {
  int value;
  int tage = 0;
  float vol = 0;
  esp_err_t err;
  err = adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &value);
  if (err == ESP_OK) {
    adc_cali_raw_to_voltage(cali_handle, value, &tage);
    vol = tage * 2;
  }
  return vol;
}

/*1表示正在充电，0表示未充电*/
uint8_t bsp_batt_get_status(void) {
  int level = gpio_get_level(GPIO_NUM_7);
  return (level == 0) ? 1 : 0;  // 0表示正在充电，1表示未充电
}
