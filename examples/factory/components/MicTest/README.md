# Mic Test

[English](README.md) | [简体中文](README_ZH.md)

The 466 × 466 round screen shows separate MIC1/MIC2 dBFS levels. They are uncalibrated digital levels, not sound-pressure measurements.

- Tap Record to capture both microphones for up to 10 seconds; Stop can finish early.
- Tap MIC1 or MIC2 to replay only the selected microphone. A new recording replaces the old one; closing the APP discards it.
- Recording uses 640000 bytes of PSRAM for 16 kHz stereo PCM16; no SD card is required.
- Raw ES7210 MIC1/MIC2 capture excludes the MIC3 playback reference. AFE, AEC, noise suppression and wake-word processing are not used here.
- Capture mutes the speaker and disables PA. Playback follows the Settings volume; this APP owns the Recorder audio session.

See the [factory guide](../../README.md) for operation.
