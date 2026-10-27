# 固件

[English](README.md) | [简体中文](README_ZH.md)

以下固件均为合并镜像，从 **0x0** 地址烧录。选择 **ESP32-S3 / 8 MB Flash**，并使用 USB 数据线连接开发板；日期最新的出厂固件列在最前。

| 文件 | 用途 |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | 新版出厂桌面，9 个 APP，集成小智语音交互 |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_v1.0.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_v1.0.bin) | 原有出厂固件 v1.0 |
| [ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin](ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin) | 原有独立小智固件 v2.2.6，会替换桌面固件 |

安装 esptool 后，在本目录运行以下命令，将 `<PORT>` 替换为电脑识别到的串口。擦除会清除原固件、配网与激活数据；切换旧版与新版时建议先完整擦除。

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

新版[出厂源码与操作说明](../examples/factory/README_ZH.md)提供各 APP 用法；不需要单独烧录语音模型或小智镜像。
