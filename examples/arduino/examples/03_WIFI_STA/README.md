# 03_WIFI_STA (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Connect to a 2.4 GHz Wi-Fi network.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable. A 2.4 GHz Wi-Fi network is required.

## How to Run

Install Arduino-ESP32 **3.3.0** in Arduino IDE. Select **ESP32S3 Dev Module**, 8 MB Flash, OPI PSRAM, USB CDC On Boot Enabled, and the Huge APP partition scheme; open `03_WIFI_STA.ino` and upload. Before uploading, change `ssid` and `password` in the sketch to your network credentials. Open the serial monitor at **115200** baud.

## Expected Result

The serial monitor shows Wi-Fi connection progress and the assigned IP address. This example does not initialize the display.

## Troubleshooting

Only 2.4 GHz Wi-Fi is supported. Replace the demonstration credentials and check the network name and password if connection fails.
