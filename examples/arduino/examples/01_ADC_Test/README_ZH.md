# 01_ADC_Test (Arduino)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

串口输出电池电压与充电状态。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。如需查看电池电压，请连接匹配的锂电池。

## 怎么运行

在 Arduino IDE 安装 Arduino-ESP32 **3.3.0**，选择 **ESP32S3 Dev Module**、8 MB Flash、OPI PSRAM、USB CDC On Boot Enabled、Huge APP 分区；打开 `01_ADC_Test.ino` 并上传。 串口监视器选择 **115200** 波特率。

## 预期现象

串口每约 2 秒显示电池电压（mV）与充电状态；此示例不初始化屏幕。

## 常见问题

未连接电池时，电压值不能用于判断电池电量；黑屏是此串口示例的正常现象。
