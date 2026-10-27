# Mic Test

[English](README.md) | [简体中文](README_ZH.md)

466 × 466 圆屏分别显示 MIC1/MIC2 的 dBFS 电平，它们是未校准的数字电平，不是声压测量值。

- 点击 Record，同时录制双麦，最长 10 秒；Stop 可提前结束。
- 分别点击 MIC1 或 MIC2，只回放所选麦克风；新录音覆盖旧录音，关闭 APP 后丢弃。
- 16 kHz、双声道 PCM16 录音使用 640000 字节 PSRAM，无需 SD 卡。
- 使用 ES7210 原始 MIC1/MIC2 模式，不包含 MIC3 回放参考，也不启用 AFE、AEC、降噪或唤醒。
- 录音时静音扬声器并关闭 PA；回放音量与设置同步。该 APP 独占 Recorder 音频会话。

完整使用方法见[出厂说明](../../README_ZH.md)。
