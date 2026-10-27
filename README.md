<div align="center">
  <h1>ESP32-S3-Touch-AMOLED-1.43C</h1>
  <p><strong>ESP32-S3 1.43-inch 466 × 466 round AMOLED touch development board</strong></p>
  <p><strong>English</strong> | <a href="README_ZH.md">简体中文</a></p>
  <p>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C/actions/workflows/build.yml"><img alt="Build" src="https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C/actions/workflows/build.yml/badge.svg"></a>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C/releases/latest"><img alt="Latest Release" src="https://img.shields.io/github/v/release/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C"></a>
    <a href="LICENSE"><img alt="License" src="https://img.shields.io/github/license/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C"></a>
  </p>
  <p><a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.43C.htm">🌐 Product</a> &middot; <a href="firmware/">📦 Firmware</a> &middot; <a href="examples/esp-idf/">🧩 ESP-IDF</a> &middot; <a href="examples/arduino/examples/">🔧 Arduino</a> &middot; <a href="hardware/">📚 Documentation</a></p>
  <p><img src="docs/images/esp32-s3-touch-amoled-1.43c.jpg" alt="ESP32-S3-Touch-AMOLED-1.43C development board" width="500"></p>
</div>

---

## ✨ Overview

This board combines a round capacitive-touch AMOLED, speaker output and dual microphones for graphical and voice applications. This repository provides basic ESP-IDF and Arduino examples, plus the complete factory desktop source and tested merged firmware.

| Feature | Device / interface |
| --- | --- |
| MCU | ESP32-S3-PICO-1-N8R8, dual core, up to 240 MHz |
| Memory | 8 MB Flash, 8 MB octal PSRAM |
| Display | 1.43-inch, 466 × 466, CO5300, QSPI |
| Touch | CST820 capacitive touch, I2C |
| Audio | ES8311 speaker output, ES7210 dual-microphone input |
| Wireless | 2.4 GHz Wi-Fi and Bluetooth LE |

## 📦 Quick Start

1. Open the [firmware folder](firmware/README.md) and download the newest dated BIN containing `FactoryOnly` in its name.
2. Connect the board with a USB data cable.
3. Follow the firmware instructions, select ESP32-S3, and flash the merged BIN at address `0x0`.
4. Reset the board and use Settings to connect to your own 2.4 GHz Wi-Fi network.

The desktop includes Display Test, Settings, Xiaozhi, Touch Test, Weather, Music, 2048, Mic Test and Button Test. See the [factory source guide](examples/factory/README.md) for operation and build instructions.

## 🧪 Examples

### ESP-IDF

| Example | Main feature |
| --- | --- |
| [01_ADC_Test](examples/esp-idf/01_ADC_Test/) | Battery voltage and charging-status output |
| [02_WIFI_AP](examples/esp-idf/02_WIFI_AP/) | 2.4 GHz Wi-Fi access point |
| [03_WIFI_STA](examples/esp-idf/03_WIFI_STA/) | Connect to a 2.4 GHz Wi-Fi network |
| [04_Audio_Test](examples/esp-idf/04_Audio_Test/) | BOOT-controlled recording, replay and built-in music |
| [05_LVGL_V8_Test](examples/esp-idf/05_LVGL_V8_Test/) | LVGL 8 widgets and automatic brightness cycle |
| [06_LVGL_V9_Test](examples/esp-idf/06_LVGL_V9_Test/) | LVGL 9 widgets and automatic brightness cycle |

### Arduino

| Example | Main feature |
| --- | --- |
| [01_ADC_Test](examples/arduino/examples/01_ADC_Test/) | Battery voltage and charging-status output |
| [02_WIFI_AP](examples/arduino/examples/02_WIFI_AP/) | 2.4 GHz Wi-Fi access point |
| [03_WIFI_STA](examples/arduino/examples/03_WIFI_STA/) | Connect to a 2.4 GHz Wi-Fi network |
| [04_Audio_Test](examples/arduino/examples/04_Audio_Test/) | BOOT-controlled recording, replay and built-in music |
| [05_LVGL_V8_Test](examples/arduino/examples/05_LVGL_V8_Test/) | LVGL 8 widgets and automatic brightness cycle |
| [06_LVGL_V9_Test](examples/arduino/examples/06_LVGL_V9_Test/) | LVGL 9 widgets and automatic brightness cycle |

Arduino libraries are in [`examples/arduino/libraries`](examples/arduino/libraries/). Install only the LVGL version specified by the example; LVGL 8 and LVGL 9 cannot be installed together in the same Arduino library folder.

## 🛠️ Development Environment

| Framework | Version | Examples |
| --- | --- | ---: |
| ESP-IDF basic examples | `v5.5.3` | 6 |
| Arduino-ESP32 | `3.3.0` | 6 |
| ESP-IDF factory source | `v5.5.4` | 1 |

## 📚 Resources

- [Schematic](hardware/schematic/)
- [Mechanical drawings](hardware/mechanical/)
- [Product page](https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.43C.htm)
- [Wiki](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.43C/)

## 🤝 Support

- Example or repository problems: [open an issue](https://github.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.43C/issues).
- Product usage, hardware or order questions: [Waveshare support](https://service.waveshare.com/).

## 📄 License

Apache License 2.0. See [LICENSE](LICENSE). Third-party components retain their own licenses.
