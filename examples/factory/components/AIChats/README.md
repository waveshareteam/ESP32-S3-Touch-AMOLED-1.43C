# ESP32-S3-Touch-AMOLED-1.43C Xiaozhi APP

Connect to Wi-Fi in Settings, then open AIChats from the desktop. On first
use, follow the activation instructions and bind the code shown on screen.
Once ready, say "你好小智" or tap Talk to start a conversation. Tap Stop to
end the current turn, or use the back arrow to return to the desktop.
Xiaozhi runs in the factory application without switching firmware or rebooting.

The 466x466 interface displays connection status, activation codes, expressions,
and conversation text. Long replies and activation instructions can be scrolled.
The ES8311 drives the speaker. Xiaozhi uses ES7210 MIC1 and MIC2 for voice and MIC3
for the hardware playback reference, with acoustic echo cancellation (AEC).
Say "你好小智" during a reply to interrupt it and start a new listening turn.
The server receives mono voice audio. Music, Mic Test and Xiaozhi share one
exclusive audio session. Exit an audio APP before opening another. Mic Test
records both microphones as 16 kHz PCM16 in PSRAM for up to 10 seconds. MIC1 and
MIC2 are replayed separately through the speaker; no SD card is required.

Build with ESP-IDF 5.5.4. The 8 MiB partition table contains a single factory
application at 0x20000 and a 1 MiB speech-model data partition at 0x700000.
The model partition contains raw ESP-SR model
data and is not mounted as a filesystem.

Read-only fonts and images remain in Flash to leave PSRAM available for the
desktop, audio buffers, and speech processing. Instruction fetching from PSRAM
remains enabled.

The AFE fetch task uses an 8 KiB PSRAM stack; the PCM input task uses a 6 KiB
PSRAM stack. Both only process audio and post control events. The control and
MCP tasks keep internal-RAM stacks for Flash/NVS access. AFE startup logs report
free and largest contiguous internal-RAM and PSRAM blocks for diagnosis.

For the first installation, run `idf.py flash` so the bootloader, partition
table, application, and `build/srmodels/srmodels.bin` are all written. Flashing
only `01_factory.bin` will not install the new partition table or wake model.
No separate Xiaozhi application image is required.
