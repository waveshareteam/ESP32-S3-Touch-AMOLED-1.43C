# 固件

[English](README.md) | [简体中文](README_ZH.md)

本目录提供最新实测出厂固件，为合并镜像，从 **0x0** 地址烧录。选择 **ESP32-S3 / 8 MB Flash**，并使用 USB 数据线连接开发板。

| 文件 | 用途 |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | 新版出厂桌面，9 个 APP，集成小智语音交互 |

安装 esptool 后，在本目录运行以下命令，将 `<PORT>` 替换为电脑识别到的串口。首次安装建议先完整擦除，这会清除原固件、配网与激活数据。

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

新版[出厂源码与操作说明](../examples/factory/README_ZH.md)提供各 APP 用法；不需要单独烧录语音模型或小智镜像。
