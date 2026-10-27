# ESP32-S3-Touch-AMOLED-1.43C Factory Demo

This firmware provides a round desktop with nine APPs, four icons per page.
It targets the 466 x 466 CO5300 AMOLED with CST820 touch, an ES8311 speaker
codec and an ES7210 dual-microphone input. Battery, camera and SD-card APPs
are not included.

## Flash the firmware

Use ESP-IDF **v5.5.4**, or flash the merged BIN in `firmware/` at address
**0x0**. Select **ESP32-S3**, **8 MB Flash**, and connect the board over USB.
The merged BIN includes the bootloader, partition table, application and
WakeNet model. Do not flash it at the application address.

The partition layout differs from the old factory demo. If you are upgrading
from that demo and NVS initialization fails, erase Flash once before flashing
the merged BIN. This clears saved Wi-Fi credentials and Xiaozhi activation data.

To compile and flash from an ESP-IDF terminal:

```sh
idf.py build
idf.py -p COM_PORT flash monitor
```

The target is already configured as `esp32s3`. Replace `COM_PORT` with the
board's actual serial port. Serial output uses **2000000 baud**.

## Connect to Wi-Fi

On first use the firmware tries the 2.4 GHz network `weather-wifi`, password
`12345678`. Previously saved credentials take priority.

To use another network, open **Settings > Connect Wi-Fi**. Scan the displayed
QR code or join the device's `S3-AMOLED-1.43C-xxxxxx` hotspot manually with
password `12345678`. Open **http://192.168.4.1** in the phone's browser and
enter your 2.4 GHz Wi-Fi credentials. Successful connections are saved.
The **Wi-Fi scan** page only lists nearby networks.

## Use the APPs

Swipe left or right on the desktop to change pages. Most APPs provide a
top-left return button. The bottom-center upward gesture opens the desktop's
navigation controls; Display Test, Touch Test and 2048 reserve gestures for
their own interaction.

| APP | How to use |
| --- | --- |
| Display Test | Cycles red, green, blue, yellow and white once per second. Double-tap anywhere on the screen, or press BOOT, to exit. |
| Settings | Adjust display brightness and speaker volume, configure Wi-Fi, or view device information. Scroll detail pages and use the top-left button to go back. |
| AIChats | Connect to Wi-Fi, follow the on-screen activation instructions if prompted, then say “你好小智” or tap Talk. Uses two microphones and the playback reference for voice processing. Volume and display brightness can also be controlled by voice. |
| Touch Test | Draw across the display with a finger. The refresh button clears the canvas. Use the top-left button or BOOT to exit. |
| Weather | Shows the configured city's current weather. Connect to Wi-Fi and tap refresh to update. The default city is Shenzhen. |
| Music | Plays the original factory demo audio stored in Flash. Tap Play/Pause; Previous and Next restart this single built-in track. No SD card or file upload is required. |
| 2048 | Swipe the game board to merge matching tiles. Use refresh to start again or the top-left button to exit. |
| Mic Test | Shows separate live levels for MIC1 and MIC2. Tap Record to capture up to 10 seconds, then Play MIC1 or Play MIC2 to hear each microphone separately. Stop can end a recording or playback early. |
| Button Test | Press and release BOOT. The page shows its state, completed press count and pass result. Test again resets the result. |

Music, Mic Test and AIChats share one audio device. Exit the current audio APP
before opening another. Mic Test mutes the speaker during recording to avoid
feedback. Recordings remain in RAM and are discarded when Mic Test closes.

## BSP layout

The BSP follows the 1.85B component layout and public interface order:

```text
components/esp32_s3_touch_amoled_1_43c/
  include/bsp/       Public pins, board, display and touch interfaces
  priv_include/      Internal interfaces
  esp32_s3_touch_amoled_1_43c.cpp   I2C, I2S, CO5300, touch and LVGL
  bsp_board.cpp      Board startup, BOOT and Wi-Fi driver initialization
  esp_lcd_touch_cst820.cpp         CST820 touch reports and reset timing
```

The CO5300 driver is a managed component. Brightness uses its native panel API;
there is no PWM backlight. The panel starts dark and becomes visible after the
first desktop frame is flushed. APP audio ownership remains in `bsp_extra`.
UI dimensions are adapted in `app_support/round_ui.hpp`, independently of BSP
hardware initialization.

Hardware reference: [1.43C schematic](https://www.waveshare.net/w/upload/5/5b/ESP32-S3-Touch-AMOLED-1.43C-Schematic.pdf).
