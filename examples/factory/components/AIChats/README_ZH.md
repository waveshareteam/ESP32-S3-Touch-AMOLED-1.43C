# 1.43C 小智 APP

[English](README.md) | [简体中文](README_ZH.md)

在设置中连接 Wi-Fi 后打开 AIChats，按屏幕说明完成激活，说“你好小智”或点击 Talk 开始对话。该 APP 在出厂桌面内运行，无需切换固件；具体操作见[出厂说明](../../README_ZH.md)。

466 × 466 界面显示状态、激活码、表情和对话文本。ES8311 负责播放；ES7210 的 MIC1/MIC2 为双麦输入，MIC3 为回放参考，供 AEC 和 WakeNet 使用。

AFE 处理任务使用 8 KiB PSRAM 栈，采音任务使用 6 KiB PSRAM 栈；Flash/NVS 操作留在使用内部 RAM 栈的控制任务和 MCP 任务中。启动日志包含内部 RAM 和 PSRAM 的可用大小及最大连续块。

使用 ESP-IDF v5.5.4；8 MiB Flash 中应用位于 0x20000，1 MiB 语音模型分区位于 0x700000。首次安装需要完整烧录，引导、分区、应用和模型必须配套。
