# 02_WIFI_AP (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

建立 2.4 GHz Wi-Fi 热点。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 ESP-IDF **v5.5.3** 终端进入本示例目录，连接开发板后运行：

```sh
idf.py build flash monitor
```

## 预期现象

手机可发现热点 `waveshare_esp32`，密码为 `wav123456`；连接/断开信息输出到串口，此示例不初始化屏幕。

## 常见问题

该热点只用于 Wi-Fi 接入测试，不提供互联网；手机提示“无互联网”是正常现象。
