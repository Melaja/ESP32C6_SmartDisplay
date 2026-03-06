# ARCHITECTURE_EN.md - Architecture Description (EN)

**Project:** ESP32C6_SMARTDISPLAY
**Version:** v2.1.0
**Author:** JWP van Renen
**Date:** 2026-03-05

---

## Overview

The project consists of a single Arduino .ino file with multiple header modules.
Each module is responsible for a well-defined part of the functionality.
The choice for header-only modules (.h) is deliberate. It avoids compile-order issues
in the Arduino/PlatformIO environment and keeps each module self-contained.

---

## System architecture (ASCII)

```
+--------------------------------------------------------------+
|                   ESP32C6_SmartDisplay.ino                   |
|  (setup + loop, initialization, tab routing)                 |
+------+----------+----------+----------+---------------------+
       |          |          |          |
       v          v          v          v
  debug_log.h  lang.h  config_manager.h  web_config.h
  (logging)  (language)  (NVS storage)  (web portal AP+STA)
       |
       +--------------------+-------------------+
       |                    |                   |
  screen_datetime.h  screen_phonefinder.h  screen_wifiscanner.h
  (Date/Time)        (Find Phone)          (WiFi Scanner)
       |                    |                   |
    LVGL v9             Telegram API        WiFi scan API
    NTP/time.h          WiFiClientSecure    esp-idf WiFi
```

---

## Module descriptions

### ESP32C6_SmartDisplay.ino

Main file. Responsible for:
- Initialization of display (GFX + JD9853 registers), LVGL, touch (AXS5106L), BLE and WiFi
- Loading configuration from NVS via `config_manager.h`
- WiFi connection or starting the configuration portal
- Building the LVGL tabview with three tabs
- Arduino `loop()`: LVGL task handler and web server requests

### debug_log.h

Central logging module. All Serial Monitor output runs exclusively through the macros
`DBG_ERROR`, `DBG_WARN`, `DBG_INFO` and `DBG_VERBOSE`. The switches `DEBUG_SERIAL` and
`DEBUG_LEVEL` are defined in the main .ino file.

### config_manager.h

Configuration management via ESP32 NVS (Non-Volatile Storage, Preferences API).
Stores WiFi credentials, Telegram settings, timezone and language in internal flash.
Contains `config_laden()`, `config_opslaan()` and `config_wissen()`.

### lang.h

Multilingual text strings (Dutch/English) via a `Lang` struct.
The global pointer `g_lang` points to the active language. All visible texts in
the screens are retrieved via `g_lang->key`.

### web_config.h

Bilingual (NL/EN) configuration portal via HTTP.
- **AP mode:** starts "ESP32-Config" access point with captive portal DNS.
  All DNS requests are redirected to 192.168.4.1.
- **STA mode:** WebServer accessible via the device's own WiFi IP address.
  Form shows current config values. After saving, the device restarts.

### screen_datetime.h

Date/Time screen. Works via an LVGL timer (1-second interval) that calls `localtime_r()`
to read the system time (synchronized via NTP) and update the labels.
Also shows the IP address and NTP synchronization status.

### screen_phonefinder.h

Find Phone screen. Sends an HTTPS GET request to the Telegram Bot API
(`api.telegram.org`) via `WiFiClientSecure`. SSL verification is disabled
(acceptable for private IoT use). A double-click guard (`pf_bezig` flag)
prevents multiple simultaneous HTTP requests.

### screen_wifiscanner.h

WiFi Scanner screen. Performs an asynchronous WiFi scan via `WiFi.scanNetworks(true)`.
An LVGL poll timer (500 ms) checks if the scan is complete. This does not block the LVGL UI.
Shows results in a scrollable list. Clicking a network opens a detail panel with
SSID, BSSID, channel, frequency, RSSI and security type.

---

## Language module

```
lang.h
  +-- struct Lang { ... }   <- all text keys
  +-- const Lang LANG_NL    <- Dutch texts
  +-- const Lang LANG_EN    <- English texts

extern const Lang* g_lang;  <- active language pointer (set in .ino)
```

---

## Configuration flow

```
[boot]
  |
  v
config_laden() from NVS
  |
  +-- configured == false or SSID empty?
  |     -> webconfig_ap_starten()  [infinite loop + reboot after save]
  |
  +-- configured == true
        -> wifi_verbinden()
        |
        +-- failed? -> webconfig_ap_starten()
        |
        +-- success?
              -> start NTP
              -> initialize BLE
              -> webconfig_sta_starten()
              -> ui_aanmaken()
              -> loop()
```

---

## Display initialization

The JD9853 display controller requires a specific sequence of SPI register commands
at startup. This sequence is stored in `display_register_init()` as a byte array
sent to the display bus via `batchOperation()`.
The `Arduino_ST7789` driver from GFX_Library_for_Arduino is used as the base class
(compatible in terms of SPI protocol).

---

## LVGL integration

- **Buffer:** Single buffer of `LCD_BREEDTE * 40` pixels in internal SRAM.
  Fallback to external memory if internal allocation fails.
- **Flush callback:** `lvgl_flush_cb()` calls `gfx->draw16bitRGBBitmap()`.
- **Tick callback:** `lvgl_tick_cb()` returns `millis()`.
- **Touch:** `lvgl_touch_read_cb()` reads coordinates via `bsp_touch_read()` (AXS5106L driver).
- **Tabview:** Three tabs at the bottom (42 px tab bar) with LVGL symbols as icons.

---

## Pin configuration

All pin definitions are exclusively in `libraries/Mylibrary/pin_config.h`.
No hardcoded pin numbers in the main .ino file or in the screen modules.

---

## Memory usage (at v2.0.0)

- **Flash:** approximately 1.6 MB of 8 MB (GFX + LVGL + WiFi + BLE)
- **SRAM heap after setup:** approximately 180-220 KB free (depending on BLE state)
- **LVGL buffer:** 172 * 40 * 2 bytes = approximately 13.8 KB

---

*Architecture documentation by JWP van Renen. Version v2.4.0, 2026-03-06.*
