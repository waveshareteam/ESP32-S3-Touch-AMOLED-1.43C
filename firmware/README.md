# Firmware

[English](README.md) | [简体中文](README_ZH.md)

These are merged images flashed at **0x0**. Select **ESP32-S3 / 8 MB Flash** and connect the board with a USB data cable. The newest factory firmware is listed first.

| File | Purpose |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | Factory desktop with nine APPs and integrated Xiaozhi voice chat |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_v1.0.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_v1.0.bin) | Original factory firmware v1.0 |
| [ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin](ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin) | Original standalone Xiaozhi v2.2.6; replaces the desktop firmware |

Install esptool, then run these commands from this folder, replacing `<PORT>` with the detected serial port. Erasing removes firmware, saved Wi-Fi and activation data; a full erase is recommended when switching between old and new firmware.

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

See the [factory source and operation guide](../examples/factory/README.md). The latest image already includes the speech model and integrated Xiaozhi APP.
