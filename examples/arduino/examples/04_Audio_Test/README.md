# 04_Audio_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

BOOT-controlled recording, replay and built-in music.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Install Arduino-ESP32 **3.3.0** in Arduino IDE. Select **ESP32S3 Dev Module**, 8 MB Flash, OPI PSRAM, USB CDC On Boot Enabled, and the Huge APP partition scheme; open `04_Audio_Test.ino` and upload. Copy `lvgl` and `lv_conf.h` from `../../libraries/lvgl9/` into your Arduino `libraries/` folder; keep only this LVGL version installed. Also copy `../../libraries/OneButton/` for the audio example. Open the serial monitor at **115200** baud.

## Expected Result

The display shows audio test status. Single-click BOOT to record about three seconds and replay automatically; double-click to play built-in music, and long-press to stop music.

## Troubleshooting

Speak near the microphones while recording and keep the speaker away from them to reduce feedback. This sketch requires LVGL 9, not LVGL 8.
