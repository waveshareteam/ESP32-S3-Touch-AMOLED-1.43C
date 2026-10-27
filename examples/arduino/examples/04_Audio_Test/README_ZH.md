# 04_Audio_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过 BOOT 按键录音、回放及播放内置音乐。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 Arduino IDE 安装 Arduino-ESP32 **3.3.0**，选择 **ESP32S3 Dev Module**、8 MB Flash、OPI PSRAM、USB CDC On Boot Enabled、Huge APP 分区；打开 `04_Audio_Test.ino` 并上传。 将 `../../libraries/` 内的 `lvgl` 和 `lv_conf.h` 复制到自己的 Arduino `libraries/`；只保留这一套 LVGL。音频示例还需复制 `../../libraries/OneButton/`。 串口监视器选择 **115200** 波特率。

## 预期现象

屏幕显示音频测试状态。单击 BOOT 录音约 3 秒并自动回放；双击 BOOT 播放内置音乐，长按 BOOT 停止音乐。

## 常见问题

如无声音，确认录音时已对着麦克风讲话，并让扬声器远离麦克风以减轻反馈。此示例使用 LVGL 9，不能安装 LVGL 9 库替代。
