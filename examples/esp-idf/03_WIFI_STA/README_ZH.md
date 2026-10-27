# 03_WIFI_STA (ESP-IDF)

[English](README.md) | [简体中文](README_ZH.md)

## 功能

连接 2.4 GHz Wi-Fi 网络。

## 硬件需求

ESP32-S3-Touch-AMOLED-1.43C 开发板与 USB 数据线。连接路由器时需要 2.4 GHz Wi-Fi。

## 怎么运行

在 ESP-IDF **v5.5.3** 终端进入本示例目录，连接开发板后运行：

```sh
idf.py build flash monitor
```

烧录前，先修改 `components/esp_wifi_bsp/esp_wifi_bsp.c` 中的 Wi-Fi 名称与密码。

## 预期现象

串口显示 Wi-Fi 连接过程和分配的 IP 地址；此示例不初始化屏幕。

## 常见问题

仅支持 2.4 GHz Wi-Fi；默认网络为演示配置，连接失败时先检查自己填写的名称和密码。
