# ESP32-S3-Touch-AMOLED-1.43C Factory Desktop

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

A 466 × 466 round desktop with four icons per page and nine APPs. Xiaozhi runs inside the same firmware with dual-microphone and playback-reference voice processing.

| APP | How to use |
| --- | --- |
| Display Test | Cycles red, green, blue, yellow and white every second; double-tap anywhere or press BOOT to exit. |
| Settings | Adjust brightness and volume, provision Wi-Fi, scan networks and view device information. |
| AIChats / Xiaozhi | Follow activation instructions if prompted, then say “你好小智” or tap Talk. Supports voice commands for volume and brightness. |
| Touch Test | Draw with a finger; clear the canvas or use the return button / BOOT to exit. |
| Weather | Connect to Wi-Fi and refresh the configured city; the default city is Shenzhen. |
| Music | Play/pause the built-in demo track. Previous and Next restart the same track; no SD card is needed. |
| 2048 | Swipe to merge matching numbers; restart or return with the on-screen controls. |
| Mic Test | View MIC1/MIC2 levels, record up to 10 seconds, and replay each microphone separately. |
| Button Test | Press and release BOOT to check its state, completed press count and pass result. |

## Hardware Required

The board and a USB data cable. Weather and Xiaozhi require a 2.4 GHz Wi-Fi network with Internet access. Music is stored in Flash and recordings use PSRAM; no SD card is needed.

## How to Run

For immediate use, download the [latest merged firmware](../../firmware/README.md) and flash it at `0x0`. To build the source, open this folder in an ESP-IDF **v5.5.4** terminal and run:

```sh
idf.py build flash monitor
```

The target is ESP32-S3 with 8 MB Flash and 8 MB PSRAM; serial logging uses **2000000** baud. The default Wi-Fi is `weather-wifi` / `12345678`. For another network, open Settings > Connect Wi-Fi, scan the QR code or join the displayed hotspot with password `12345678`, and open `http://192.168.4.1` to enter your own network credentials.

## Expected Result

The desktop appears after startup. Swipe left or right to change pages and tap an icon to open an APP; most pages have a top-left return button. Mic Test shows separate levels and allows individual MIC1 / MIC2 replay after recording; closing the APP discards the recording.

## Troubleshooting

Music, Mic Test and Xiaozhi share one audio device: exit the current audio APP before opening another. For the first installation, flash the complete merged image or run the full `idf.py flash`; flashing only the application BIN does not install the partition table and speech model.
