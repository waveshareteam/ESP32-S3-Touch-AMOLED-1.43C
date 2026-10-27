# Firmware

[English](README.md) | [简体中文](README_ZH.md)

The latest tested factory firmware is a merged image flashed at **0x0**. Select **ESP32-S3 / 8 MB Flash** and connect the board with a USB data cable.

| File | Purpose |
| --- | --- |
| [ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin](ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin) | Factory desktop with nine APPs and integrated Xiaozhi voice chat |

Install esptool, then run these commands from this folder, replacing `<PORT>` with the detected serial port. A full erase is recommended for the first installation; it removes firmware, saved Wi-Fi and activation data.

```sh
python -m pip install esptool
python -m esptool --chip esp32s3 --port <PORT> erase_flash
python -m esptool --chip esp32s3 --port <PORT> write_flash 0x0 ESP32-S3-Touch-AMOLED-1.43C_FactoryOnly_20261027.bin
```

See the [factory source and operation guide](../examples/factory/README.md). The latest image already includes the speech model and integrated Xiaozhi APP.
