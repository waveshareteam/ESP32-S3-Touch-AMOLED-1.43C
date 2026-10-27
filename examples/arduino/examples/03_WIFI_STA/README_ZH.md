# 03_WIFI_STA (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

连接 2.4 GHz Wi-Fi 网络。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。连接路由器时需要 2.4 GHz Wi-Fi。

## 怎么运行

在 Arduino IDE 安装 Arduino-ESP32 **3.3.0**，选择 **ESP32S3 Dev Module**、8 MB Flash、OPI PSRAM、USB CDC On Boot Enabled、Huge APP 分区；打开 `03_WIFI_STA.ino` 并上传。 上传前将草图中的 `ssid` 和 `password` 改为自己的网络。 串口监视器选择 **115200** 波特率。

## 预期现象

串口显示 Wi-Fi 连接过程和分配的 IP 地址；此示例不初始化屏幕。

## 常见问题

仅支持 2.4 GHz Wi-Fi；默认网络为演示配置，连接失败时先检查自己填写的名称和密码。
