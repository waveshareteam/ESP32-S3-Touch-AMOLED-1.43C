# 02_WIFI_AP (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

2.4 GHz Wi-Fi access point.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Install Arduino-ESP32 **3.3.0** in Arduino IDE. Select **ESP32S3 Dev Module**, 8 MB Flash, OPI PSRAM, USB CDC On Boot Enabled, and the Huge APP partition scheme; open `02_WIFI_AP.ino` and upload. Open the serial monitor at **115200** baud.

## Expected Result

A phone can join `ESP32_AP` with password `12345678`. Serial output shows station events; this example does not initialize the display.

## Troubleshooting

This is a local Wi-Fi access-point test, not an Internet gateway. A phone may correctly report that the network has no Internet access.
