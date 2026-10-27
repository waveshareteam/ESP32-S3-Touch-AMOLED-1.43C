# 05_LVGL_V8_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 8 控件演示与自动亮度循环。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 Arduino IDE 安装 Arduino-ESP32 **3.3.0**，选择 **ESP32S3 Dev Module**、8 MB Flash、OPI PSRAM、USB CDC On Boot Enabled、Huge APP 分区；打开 `05_LVGL_V8_Test.ino` 并上传。 将 `../../libraries/lvgl8/` 内的 `lvgl` 和 `lv_conf.h` 复制到自己的 Arduino `libraries/`；只保留这一套 LVGL。 串口监视器选择 **115200** 波特率。

## 预期现象

屏幕显示可触摸操作的 LVGL 8 控件；亮度每 2 秒在 100%、75%、50%、25%、0% 间循环。

## 常见问题

亮度循环到 0% 时屏幕会短暂变黑，等待下一轮即可。此示例必须使用 LVGL 8，不能与 LVGL 9 库同时安装。
