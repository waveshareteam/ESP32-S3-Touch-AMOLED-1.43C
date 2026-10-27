# 01_ADC_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

串口输出电池电压与充电状态。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。如需查看电池电压，请连接匹配的锂电池。

## 怎么运行

在 ESP-IDF **v5.5.3** 终端进入本示例目录，连接开发板后运行：

```sh
idf.py build flash monitor
```

## 预期现象

串口每约 2 秒显示电池电压（mV）与充电状态；此示例不初始化屏幕。

## 常见问题

未连接电池时，电压值不能用于判断电池电量；黑屏是此串口示例的正常现象。
