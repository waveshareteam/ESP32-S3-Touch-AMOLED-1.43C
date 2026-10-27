# 02_WIFI_AP (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

建立 2.4 GHz Wi-Fi 热点。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 Arduino IDE 安装 Arduino-ESP32 **3.3.0**，选择 **ESP32S3 Dev Module**、8 MB Flash、OPI PSRAM、USB CDC On Boot Enabled、Huge APP 分区；打开 `02_WIFI_AP.ino` 并上传。 串口监视器选择 **115200** 波特率。

## 预期现象

手机可发现热点 `ESP32_AP`，密码为 `12345678`；连接/断开信息输出到串口，此示例不初始化屏幕。

## 常见问题

该热点只用于 Wi-Fi 接入测试，不提供互联网；手机提示“无互联网”是正常现象。
