# 02_WIFI_AP (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

2.4 GHz Wi-Fi access point.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Open this example folder in an ESP-IDF **v5.5.3** terminal, connect the board, and run:

```sh
idf.py build flash monitor
```

## Expected Result

A phone can join `waveshare_esp32` with password `wav123456`. Serial output shows station events; this example does not initialize the display.

## Troubleshooting

This is a local Wi-Fi access-point test, not an Internet gateway. A phone may correctly report that the network has no Internet access.
