# 05_LVGL_V8_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

LVGL 8 控件演示与自动亮度循环。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 ESP-IDF **v5.5.3** 终端进入本示例目录，连接开发板后运行：

```sh
idf.py build flash monitor
```

## 预期现象

屏幕显示可触摸操作的 LVGL 8 控件；亮度每 2 秒在 100%、75%、50%、25%、0% 间循环。

## 常见问题

亮度循环到 0% 时屏幕会短暂变黑，等待下一轮即可。
