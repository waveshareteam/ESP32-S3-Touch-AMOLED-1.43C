# 05_LVGL_V8_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

LVGL 8 widgets and automatic brightness cycle.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Open this example folder in an ESP-IDF **v5.5.3** terminal, connect the board, and run:

```sh
idf.py build flash monitor
```

## Expected Result

The display shows interactive LVGL 8 widgets. Brightness cycles through 100%, 75%, 50%, 25% and 0%, changing every two seconds.

## Troubleshooting

The screen briefly goes dark at 0% brightness; wait for the next cycle.
