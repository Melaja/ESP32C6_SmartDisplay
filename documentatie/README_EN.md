# ESP32C6 SmartDisplay - Documentation (EN)

**Project:** ESP32C6_SMARTDISPLAY
**Version:** v2.0.0
**Author:** JWP van Renen
**Date:** 2026-03-05

---

## Project Overview

ESP32C6_SmartDisplay is a multifunctional dashboard for the Waveshare ESP32-C6-Touch-LCD-1.47.
It displays three interactive touch screens powered by LVGL v9:

1. **Date/Time** - Date in Dutch, NTP-synchronized time
2. **Find Phone** - Sends a Telegram message to locate your phone
3. **WiFi Scanner** - Scans WiFi networks with detailed network information

On first boot (or if the WiFi connection fails), the device starts as an access point named
"ESP32-Config". Connect to that WiFi network and open 192.168.4.1 in a browser to configure
WiFi, Telegram and timezone. After saving, the device restarts automatically.
The configuration portal is also accessible via http://[IP-address]/ on your own WiFi.

---

## .ino filename

`ESP32C6_SmartDisplay.ino`

---

## Hardware overview

| Component | Specification |
|-----------|--------------|
| Board | Waveshare ESP32-C6-Touch-LCD-1.47 |
| Chip | ESP32-C6FH8 (RISC-V, 160 MHz, 8 MB Flash, 512 KB SRAM) |
| Display | 1.47" IPS, 172x320 px, JD9853 controller, SPI |
| Touch | Capacitive, AXS5106L controller, I2C (address 0x63) |
| Connectivity | WiFi 6 (802.11ax, 2.4 GHz), Bluetooth 5 BLE |
| Power | USB-C, 5 V, typically 250 mA (without BLE scan), max 400 mA |

---

## Board settings (PlatformIO)

| Parameter | Value |
|-----------|-------|
| Platform | espressif32 |
| Board | esp32-c6-devkitc-1 |
| Framework | arduino |
| MCU | esp32c6 |
| CPU Frequency | 160 MHz |
| Flash size | 8 MB |
| Flash frequency | 80 MHz |
| Flash mode | QIO |
| Partition scheme | default_8MB.csv |
| Upload speed | 921600 |
| Monitor speed | 115200 |
| Loop Stack | 16384 bytes |

---

## External libraries

| Library Name | Version | Source | Notes |
|--------------|---------|--------|-------|
| lvgl | 9.5.0 | Local in libraries/lvgl/ | UI framework |
| GFX_Library_for_Arduino | 1.5.9 | Local in libraries/GFX_Library_for_Arduino/ | Display driver |
| esp_lcd_touch_axs5106l | 1.0.0 | Local in libraries/esp_lcd_touch_axs5106l/ | Touch driver |
| espressif32 (PlatformIO platform) | 3.0.0+ | PlatformIO registry | ESP32 board package |
| Network (arduino-esp32) | built-in | framework-arduinoespressif32 | WiFi stack fix |

---

## Installation and compilation

### Requirements

- [PlatformIO IDE](https://platformio.org/) (recommended) or Arduino IDE 2.x
- ESP32-C6 USB-C cable

### Steps (PlatformIO)

1. Clone or download the project to a local directory.
2. Open the project folder in VSCode with PlatformIO.
3. Copy `src/wifi_config_example.h` to `src/wifi_config.h`.
4. Fill in your WiFi credentials in `src/wifi_config.h` (only needed for initial testing without the web portal).
5. Click **Build** (checkmark) to compile.
6. Connect the ESP32-C6 via USB-C.
7. Click **Upload** (arrow) to upload (921600 baud).
8. Open the Serial Monitor (115200 baud) for debug output.

### First-time configuration

After the first upload, the device starts as access point "ESP32-Config".
Connect to that network and navigate to 192.168.4.1. Enter WiFi credentials, Telegram token,
chat ID and timezone. After saving, the device restarts and connects to your WiFi network.

---

## Configuration via web portal

The web portal is bilingual (Dutch/English). Available settings:

| Setting | Description |
|---------|------------|
| WiFi SSID | Name of your WiFi network |
| WiFi Password | Password of your WiFi network |
| Telegram Token | Bot token from BotFather (e.g. 123456:ABCdef...) |
| Telegram Chat ID | Your numeric chat ID or @username |
| Timezone | POSIX timezone (default: CET-1CEST,M3.5.0,M10.5.0/3) |
| Language | nl (Dutch) or en (English) |

---

## Troubleshooting

| Problem | Possible cause | Solution |
|---------|---------------|---------|
| Screen stays black | Display init failed | Check SPI wiring, restart |
| WiFi won't connect | Wrong credentials | Connect to "ESP32-Config", reconfigure |
| NTP shows "--:--:--" | No WiFi or NTP timeout | Wait up to 30 seconds after connection |
| Telegram fails (401) | Invalid token | Check token via BotFather |
| Telegram fails (400) | Wrong chat ID | Check chat ID via @userinfobot |
| Touch not responding | I2C problem | Check SDA/SCL wiring, restart |

---

## Debug on/off

In `src/ESP32C6_SmartDisplay.ino`:

```cpp
#define DEBUG_SERIAL 1   // 1 = debug on, 0 = debug off
#define DEBUG_LEVEL  3   // 0=off, 1=errors, 2=warnings, 3=info, 4=verbose
```

When `DEBUG_SERIAL 0`, all logging is disabled regardless of `DEBUG_LEVEL`.

---

## Library structure

```
ESP32C6_SmartDisplay/
  src/
    ESP32C6_SmartDisplay.ino   <- Main file
    debug_log.h                <- Logging macros
    config_manager.h           <- NVS configuration management
    lang.h                     <- Language strings (NL/EN)
    web_config.h               <- Web portal (AP + STA)
    screen_datetime.h          <- Date/Time screen
    screen_phonefinder.h       <- Find Phone screen
    screen_wifiscanner.h       <- WiFi Scanner screen
    wifi_config_example.h      <- Example file (NEVER commit wifi_config.h)
  libraries/
    lv_conf.h                  <- LVGL configuration
    Mylibrary/
      pin_config.h             <- Pin definitions
    lvgl/                      <- LVGL v9.5.0
    GFX_Library_for_Arduino/   <- GFX v1.5.9
    esp_lcd_touch_axs5106l/    <- Touch driver v1.0.0
  opdracht/
    opdracht.md                <- Functional requirements
  documentatie/                <- This document
  platformio.ini               <- PlatformIO build configuration
  .gitignore
```

---

## Fail-safe behavior

| Situation | Behavior |
|-----------|---------|
| Display init fails | Error log, restart after 5 seconds |
| LVGL buffer allocation fails | Error log, restart after 5 seconds |
| WiFi unavailable | Configuration portal as access point |
| NTP not synchronized | Shows "--:--:--", retries every second |
| Telegram token/chat ID empty | Error message on screen, no HTTP request |
| WiFi lost during Telegram | Error message on screen |

---

*Documentation generated with support of Co-worker. Version v2.0.0, 2026-03-05.*
