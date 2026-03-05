# BOM.md - Bill of Materials

**Project:** ESP32C6_SMARTDISPLAY
**Versie:** v2.0.0
**Auteur:** JWP van Renen
**Datum:** 2026-03-05

---

## Hardware componenten

| Component | Type / Model | Aantal | Opmerking |
|-----------|-------------|--------|-----------|
| Ontwikkelbord | Waveshare ESP32-C6-Touch-LCD-1.47 | 1 | Inclusief display en touch |
| USB-C kabel | USB-A naar USB-C of USB-C naar USB-C | 1 | Voor programmeren en voeding |
| USB-voeding | 5 V, min. 500 mA | 1 | Telefoonoplader of USB-poort |

### Geintegreerde onderdelen (op het bord)

| Onderdeel | Type | Opmerking |
|-----------|------|-----------|
| Microcontroller | ESP32-C6FH8 (RISC-V, 160 MHz) | 8 MB Flash, 512 KB SRAM |
| Display | 1.47" IPS TFT, 172x320 px | Controller: JD9853, interface: SPI |
| Touch IC | AXS5106L | Capacitief, I2C adres 0x63 |
| WiFi | WiFi 6 (802.11ax, 2.4 GHz) | Ingebouwd in ESP32-C6 |
| Bluetooth | BLE 5 | Ingebouwd in ESP32-C6 |
| RGB LED | WS2812 of vergelijkbaar | Aanwezig op het bord |

---

## Software afhankelijkheden

| Library | Versie | Licentie | Bron |
|---------|--------|---------|------|
| lvgl | 9.5.0 | MIT | Lokaal in libraries/lvgl/ |
| GFX_Library_for_Arduino | 1.5.9 | BSD-like | Lokaal in libraries/GFX_Library_for_Arduino/ |
| esp_lcd_touch_axs5106l | 1.0.0 | MIT/Apache | Lokaal in libraries/esp_lcd_touch_axs5106l/ |
| espressif32 (PlatformIO) | 3.0.0+ | Apache 2.0 | PlatformIO registry |
| arduino-esp32 framework | ingebouwd | LGPL/Apache | Via espressif32 platform |

---

## Alternatieve hardware

| Alternatief | Geschikt? | Opmerkingen |
|------------|----------|------------|
| Waveshare ESP32-S3-Touch-LCD-1.47 | Nee | Andere MCU, andere pinout |
| Generieke ESP32-C6 dev board + extern display | Nee | Vereist aanpassing pin_config.h en display driver |
| Waveshare ESP32-C6-Touch-LCD-1.47 (andere batch) | Ja | Controleer revisienummer voor eventuele pinwijzigingen |

---

*BOM gegenereerd met ondersteuning van Co-worker. Versie v2.0.0, 2026-03-05.*
