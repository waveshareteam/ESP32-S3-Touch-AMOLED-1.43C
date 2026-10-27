# Firmware

[English](README.md) | [简体中文](README_ZH.md)

Download the latest factory firmware below to try the desktop and its APPs. All files are complete merged images flashed at **0x0**; select **ESP32-S3 / 8 MB Flash** and connect the board with a USB data cable.

| File | Purpose |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | Recommended: desktop with nine APPs, including display, touch, audio tests and Xiaozhi voice chat |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20260420.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20260420.bin) | Factory demo v1.0; restores the earlier factory interface |
| [ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin](ESP32-S3-Touch-AMOLED-1.43C_XiaoZhi_v2.2.6.bin) | Standalone Xiaozhi voice chat v2.2.6; replaces the desktop with the Xiaozhi application |

Install esptool, then run these commands from this folder, replacing `<PORT>` with the detected serial port. A full erase is recommended for the first installation; it removes firmware, saved Wi-Fi and activation data.

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

See the [factory source and operation guide](../examples/factory/README.md). The latest image already includes the speech model and integrated Xiaozhi APP.
