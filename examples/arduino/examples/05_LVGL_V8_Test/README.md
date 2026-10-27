# 05_LVGL_V8_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

LVGL 8 widgets and automatic brightness cycle.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Install Arduino-ESP32 **3.3.0** in Arduino IDE. Select **ESP32S3 Dev Module**, 8 MB Flash, OPI PSRAM, USB CDC On Boot Enabled, and the Huge APP partition scheme; open `05_LVGL_V8_Test.ino` and upload. Copy `lvgl` and `lv_conf.h` from `../../libraries/lvgl8/` into your Arduino `libraries/` folder; keep only this LVGL version installed. Open the serial monitor at **115200** baud.

## Expected Result

The display shows interactive LVGL 8 widgets. Brightness cycles through 100%, 75%, 50%, 25% and 0%, changing every two seconds.

## Troubleshooting

The screen briefly goes dark at 0% brightness; wait for the next cycle. Install LVGL 8 only for this sketch, not LVGL 9.
