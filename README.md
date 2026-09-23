# ESP32 Portable Audio Player

> **Status: In Progress**

A portable digital audio player built around the ESP32-WROVER-E. The project combines microSD storage, a TFT user interface, physical controls, I2S digital audio output, and battery-powered hardware into a standalone embedded audio system.

The long-term goal is to support WAV, MP3, and FLAC playback with a custom user interface, portable power system, and eventually a custom PCB and enclosure.

---

## Current Progress

### Implemented

- microSD card initialization and filesystem navigation
- Folder and file browsing from the `/music` directory
- TFT menu interface using an ST7789 display
- Rotary encoder navigation and physical button input    
- I2S audio output configuration
- Verified audio output using a generated PCM test tone
- Dedicated ESP32 audio task using FreeRTOS
- Battery-powered system with a soft-latch power circuit
- Modular firmware separated into audio, display, controls, power, and SD-card components

### In Progress

- Audio file decoding and playback
- Connecting selected files from the user interface to the audio playback system
- Playback controls and now-playing interface
- Battery monitoring and status display

### Planned

- WAV, MP3, and FLAC playback
- Play/pause, next, previous, shuffle, and repeat controls
- Playlist and album navigation
- Custom PCB
- 3D-printed enclosure
- Possible Bluetooth audio support

---

## System Architecture

```text
                    ┌──────────────┐
                    │   microSD    │
                    │    Card      │
                    └──────┬───────┘
                           │
                           ▼
Physical Controls ──► ESP32-WROVER-E ──► TFT Display
                           │
                           │ I2S
                           ▼
                      PCM5102 DAC
                           │
                           ▼
                    Headphone Amplifier
                           │
                           ▼
                       Headphones
