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

```

## Power System

```text
Battery
   │
   ▼
Charging / Protection
   │
   ▼
Boost Converter
   │
   ▼
Soft-Latch Power Circuit
   │
   ▼
ESP32 + Display + Audio Hardware
```

## Hardware

- ESP32-WROVER-E
- ST7789 TFT display
- PCM5102 I2S DAC
- Headphone amplifier
- microSD card
- Rotary encoder
- Physical push buttons
- Li-ion battery
- TP4056 charging/protection module
- Boost converter
- MOSFET-based soft-latch power circuit

## Software
- C++
- Arduino framework
- PlatformIO
- FreeRTOS
- TFT_eSPI
- I2S
- SPI
- SD_MMC
- Git / GitHub

## Main Modules
- audioPlayer — I2S configuration, audio generation/output, and FreeRTOS audio task
- controls — Rotary encoder and button input handling
- displayUI — TFT rendering, menu navigation, cursor movement, and file display
- power — Power sequencing and soft-latch control
- sd_card — microSD initialization, filesystem access, and directory listing
- main — System initialization and high-level application flow

## Development Process
This project is being developed incrementally, beginning with individual hardware and firmware subsystems before combining them into a complete portable audio player.

Development has included:

- Designing and debugging the power system
- Bringing up the TFT display and user interface
- Implementing microSD filesystem navigation
- Integrating rotary encoder and button controls
- Configuring I2S communication with the external DAC
- Testing audio output with generated PCM data
- Beginning use of FreeRTOS to separate audio processing from the main interface logic

A more detailed development log can be found here:

[Development Log](https://docs.google.com/document/d/1O5QRbn1U3DH5DhtzLS91JwxgiUE57WrMUlMddMPFr3o/edit?usp=sharing)

## Future Development

The next major milestone is decoding audio files stored on the microSD card and streaming PCM audio through the I2S DAC.

After reliable playback is implemented, the project will move toward:

- Playback controls and now-playing UI
- Improved battery monitoring and power management
- PCB design
- Enclosure design
- Additional audio features such as playlists, shuffle, repeat, and possibly Bluetooth output
