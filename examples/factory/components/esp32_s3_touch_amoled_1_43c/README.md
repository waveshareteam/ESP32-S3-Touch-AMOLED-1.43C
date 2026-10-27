# ESP32-S3-Touch-AMOLED-1.43C BSP

Board support for the 466 x 466 CO5300 QSPI AMOLED, CST820 touch,
ES8311 playback, ES7210 dual-microphone capture and BOOT GPIO0.
The component follows the 1.85B BSP directory layout and common API order.

Include `bsp/esp-bsp.h`. Use `bsp_init()` for the factory desktop, then inspect
`bsp_get_init_status()` for peripheral availability. Create the desktop before
calling `bsp_display_show()`: AMOLED brightness stays at zero until that call
flushes the first frame. Brightness uses the CO5300 native panel API.

Simple demos may call individual initialization functions. For example,
`bsp_display_start()` initializes the shared I2C bus, display and touch; it does
not start audio, the button driver or Wi-Fi. Wi-Fi credentials and provisioning
belong to `network_service`, while APP audio ownership belongs to `bsp_extra`.

Public interfaces:

- `include/bsp/esp32_s3_touch_amoled_1_43c.h`: pins, I2C, I2S and LVGL startup.
- `include/bsp/board.h`: initialization status, BOOT, Wi-Fi and handle access.
- `include/bsp/display.h`: panel creation, brightness and first-frame display.
- `include/bsp/touch.h`: touch creation.

Implementation order remains I2C, I2S/codecs, brightness/display, touch and LVGL.
`bsp_board.cpp` contains board startup, BOOT and Wi-Fi driver initialization.
`esp_lcd_touch_cst820.cpp` contains the module reset timing and touch reports.

The audio LDO is enabled through GPIO18 before codec access. ES7210 MIC1 and
MIC2 are the physical microphones; MIC3 receives the DAC playback reference.
Mic Test captures the two microphones directly. Xiaozhi uses the two
microphones plus playback reference for AEC and WakeNet.

Use `bsp_display_lock()` / `bsp_display_unlock()` around LVGL access from other
tasks. A timeout of `0` or `UINT32_MAX` waits indefinitely. Stop audio workers
before deinitializing the BSP-owned codecs or I2S handles.

The default LVGL task uses a 16 KB internal-RAM stack, configured by
`CONFIG_BSP_LVGL_TASK_STACK_SIZE`. The desktop reports the minimum remaining
stack after rendering. Its startup main task uses 12 KB because initial UI
creation and the first synchronous refresh execute on that task.
