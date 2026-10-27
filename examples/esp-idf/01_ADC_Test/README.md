# 01_ADC_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Battery voltage and charging-status output.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable. Connect a compatible battery to measure its voltage.

## How to Run

Open this example folder in an ESP-IDF **v5.5.3** terminal, connect the board, and run:

```sh
idf.py build flash monitor
```

## Expected Result

The serial monitor prints battery voltage in mV and charging status about every two seconds. This example does not initialize the display.

## Troubleshooting

Without a connected battery, voltage readings do not indicate battery charge. A dark display is normal for this serial-only example.
