#include <Arduino.h>
#include "esp32_s3_touch_amoled_1.43c.h"

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.printf("adc-example run\n");
  bsp_batt_init();
}
void loop() {
  Serial.printf("Voltage:%dmV,Charging-Status:%s\n",bsp_batt_get_voltage(),bsp_batt_get_status() ? "Charging" : "No Charging");
  delay(2000);
}
