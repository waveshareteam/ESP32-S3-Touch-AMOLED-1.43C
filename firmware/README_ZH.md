# 固件

[English](README.md) | [简体中文](README_ZH.md)

首次使用推荐下载下表中的最新出厂固件，体验桌面及各项 APP。所有文件均为完整合并固件，从 **0x0** 地址烧录；选择 **ESP32-S3 / 8 MB Flash**，并使用 USB 数据线连接开发板。

| 文件 | 用途 |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | 推荐使用：包含显示、触摸、音频测试等 9 个 APP 的桌面，支持小智语音对话 |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20260420.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20260420.bin) | 出厂演示固件 v1.0，用于恢复早期出厂界面 |
| [ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin](ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin) | 独立小智语音对话固件 v2.2.6，烧录后进入小智应用，替换桌面固件 |

安装 esptool 后，在本目录运行以下命令，将 `<PORT>` 替换为电脑识别到的串口。首次安装建议先完整擦除，这会清除原固件、配网与激活数据。

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

新版[出厂源码与操作说明](../examples/factory/README_ZH.md)提供各 APP 用法；不需要单独烧录语音模型或小智镜像。
