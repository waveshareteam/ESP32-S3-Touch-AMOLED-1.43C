# 04_Audio_Test (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过 BOOT 按键录音、回放及播放内置音乐。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。

## 怎么运行

在 ESP-IDF **v5.5.3** 终端进入本示例目录，连接开发板后运行：

```sh
idf.py build flash monitor
```

## 预期现象

屏幕显示音频测试状态。单击 BOOT 录音约 3 秒并自动回放；双击 BOOT 播放内置音乐，长按 BOOT 停止音乐。

## 常见问题

如无声音，确认录音时已对着麦克风讲话，并让扬声器远离麦克风以减轻反馈。
