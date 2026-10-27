# 1.43C BSP

[English](README.md) | [简体中文](README_ZH.md)

BSP 统一封装 CO5300、CST820、ES8311/ES7210、BOOT 与共享总线。公共头文件位于 `include/bsp/`，内部接口位于 `priv_include/`；音频 APP 的互斥所有权由 `bsp_extra` 管理。

包含 `bsp/esp-bsp.h` 后，可调用 `bsp_init()` 初始化整板，也可按需调用单项初始化函数。`bsp_display_start()` 只初始化 I2C、显示和触摸；创建桌面后调用 `bsp_display_show()`，首帧刷新完成才显示 AMOLED 亮度。

GPIO18 先使能音频 LDO。MIC1/MIC2 是物理双麦，MIC3 是播放参考；Mic Test 读取原始双麦，小智使用双麦与参考进行语音前处理。跨任务访问 LVGL 时使用显示锁，释放音频设备前先停止音频工作任务。

LVGL 默认使用 16 KiB 内部 RAM 栈，启动主任务使用 12 KiB 栈；桌面日志输出 LVGL 任务的最小剩余栈。硬件引脚以公共 BSP 头文件和[原理图](../../../../hardware/schematic/)为准。
