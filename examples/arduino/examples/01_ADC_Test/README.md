# 01_ADC_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Battery voltage and charging-status output.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable. Connect a compatible battery to measure its voltage.

## How to Run

Install Arduino-ESP32 **3.3.0** in Arduino IDE. Select **ESP32S3 Dev Module**, 8 MB Flash, OPI PSRAM, USB CDC On Boot Enabled, and the Huge APP partition scheme; open `01_ADC_Test.ino` and upload. Open the serial monitor at **115200** baud.

## Expected Result

The serial monitor prints battery voltage in mV and charging status about every two seconds. This example does not initialize the display.

## Troubleshooting

Without a connected battery, voltage readings do not indicate battery charge. A dark display is normal for this serial-only example.
