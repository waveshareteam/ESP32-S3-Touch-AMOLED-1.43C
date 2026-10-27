# 04_Audio_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

BOOT-controlled recording, replay and built-in music.

## Hardware Required

ESP32-S3-Touch-AMOLED-1.43C and a USB data cable.

## How to Run

Open this example folder in an ESP-IDF **v5.5.3** terminal, connect the board, and run:

```sh
idf.py build flash monitor
```

## Expected Result

The display shows audio test status. Single-click BOOT to record about three seconds and replay automatically; double-click to play built-in music, and long-press to stop music.

## Troubleshooting

Speak near the microphones while recording and keep the speaker away from them to reduce feedback.
