# 03_WIFI_STA (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Connect to a 2.4 GHz Wi-Fi network.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable. A 2.4 GHz Wi-Fi network is required.

## How to Run

Open this example folder in an ESP-IDF **v5.5.3** terminal, connect the board, and run:

```sh
idf.py build flash monitor
```

Before flashing, set your Wi-Fi name and password in `components/esp_wifi_bsp/esp_wifi_bsp.c`.

## Expected Result

The serial monitor shows Wi-Fi connection progress and the assigned IP address. This example does not initialize the display.

## Troubleshooting

Only 2.4 GHz Wi-Fi is supported. Replace the demonstration credentials and check the network name and password if connection fails.
