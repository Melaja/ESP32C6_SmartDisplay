# ESP32C6 SmartDisplay - Documentatie (NL)

**Project:** ESP32C6_SMARTDISPLAY
**Versie:** v2.1.0
**Auteur:** JWP van Renen
**Datum:** 2026-03-05

---

## Projectoverzicht

ESP32C6_SmartDisplay is een multifunctioneel dashboard voor de Waveshare ESP32-C6-Touch-LCD-1.47.
Het toont drie interactieve touchschermen via LVGL v9:

1. **Datum/Tijd** - Datum in het Nederlands, gesynchroniseerde tijd via NTP
2. **Zoek Telefoon** - Verstuurt een Telegram-bericht om uw telefoon te vinden
3. **WiFi Scanner** - Scant WiFi-netwerken met gedetailleerde netwerkinformatie

Bij de eerste opstart (of als de WiFi-verbinding mislukt) start het apparaat als accesspoint
"ESP32-Config". Verbind met dat WiFi-netwerk en open 192.168.4.1 in een browser om WiFi,
Telegram en tijdzone in te stellen. Na opslaan herstart het apparaat automatisch.
Nadien is het configuratieportaal ook bereikbaar via http://[IP-adres]/ op uw eigen WiFi.

---

## Bestandsnaam van de .ino

`ESP32C6_SmartDisplay.ino`

---

## Hardware overzicht

| Component | Specificatie |
|-----------|-------------|
| Bord | Waveshare ESP32-C6-Touch-LCD-1.47 |
| Chip | ESP32-C6FH8 (RISC-V, 160 MHz, 8 MB Flash, 512 KB SRAM) |
| Display | 1.47" IPS, 172x320 px, JD9853 controller, SPI |
| Touch | Capacitief, AXS5106L controller, I2C (adres 0x63) |
| Connectiviteit | WiFi 6 (802.11ax, 2.4 GHz), Bluetooth 5 BLE |
| Voeding | USB-C, 5 V, typisch 250 mA (zonder BLE-scan), max 400 mA |

---

## Bordinstellingen (PlatformIO)

| Parameter | Waarde |
|-----------|--------|
| Platform | espressif32 |
| Board | esp32-c6-devkitc-1 |
| Framework | arduino |
| MCU | esp32c6 |
| CPU Frequentie | 160 MHz |
| Flash grootte | 8 MB |
| Flash frequentie | 80 MHz |
| Flash modus | QIO |
| Partitieschema | default_8MB.csv |
| Upload snelheid | 921600 |
| Monitor snelheid | 115200 |
| Loop Stack | 16384 bytes |

---

## Externe libraries

| Library Naam | Versie | Bron | Opmerking |
|--------------|--------|------|-----------|
| lvgl | 9.5.0 | Lokaal in libraries/lvgl/ | UI framework |
| GFX_Library_for_Arduino | 1.5.9 | Lokaal in libraries/GFX_Library_for_Arduino/ | Display driver |
| esp_lcd_touch_axs5106l | 1.0.0 | Lokaal in libraries/esp_lcd_touch_axs5106l/ | Touch driver |
| espressif32 (PlatformIO platform) | 3.0.0+ | PlatformIO registry | ESP32 board package |
| Network (arduino-esp32) | ingebouwd | framework-arduinoespressif32 | WiFi stack fix |

---

## Installatie en compilatie

### Vereisten

- [PlatformIO IDE](https://platformio.org/) (aanbevolen) of Arduino IDE 2.x
- ESP32-C6 USB-C kabel

### Stappen (PlatformIO)

1. Clone of download het project naar een lokale map.
2. Open de projectmap in VSCode met PlatformIO.
3. Kopieer `src/wifi_config_example.h` naar `src/wifi_config.h`.
4. Vul uw WiFi-gegevens in `src/wifi_config.h` in (alleen voor initieel testen zonder webportaal).
5. Klik op **Build** (vinkje) om te compileren.
6. Verbind de ESP32-C6 via USB-C.
7. Klik op **Upload** (pijltje) om te uploaden (921600 baud).
8. Open de Serial Monitor (115200 baud) voor debug-uitvoer.

### Eerste configuratie

Na de eerste upload start het apparaat als accesspoint "ESP32-Config".
Verbind met dat netwerk en ga naar 192.168.4.1. Vul WiFi, Telegram-token,
chat-ID en tijdzone in. Na opslaan herstart het apparaat en verbindt het met uw WiFi.

---

## Configuratie via webportaal

Het webportaal is tweetalig (Nederlands/Engels). Beschikbare instellingen:

| Instelling | Beschrijving |
|-----------|-------------|
| WiFi SSID | Naam van uw WiFi-netwerk |
| WiFi Wachtwoord | Wachtwoord van uw WiFi-netwerk |
| Telegram Token | Bot-token van BotFather (bijv. 123456:ABCdef...) |
| Telegram Chat ID | Uw numerieke chat-ID of @gebruikersnaam |
| Tijdzone | POSIX-tijdzone (standaard: CET-1CEST,M3.5.0,M10.5.0/3) |
| Taal | nl (Nederlands) of en (English) |

---

## Troubleshooting

| Probleem | Mogelijke oorzaak | Oplossing |
|---------|------------------|----------|
| Scherm blijft zwart | Display init mislukt | Controleer SPI-kabels, herstart |
| WiFi verbindt niet | Verkeerde gegevens | Verbind met "ESP32-Config", herconfigureer |
| NTP toont "--:--:--" | Geen WiFi of NTP-timeout | Wacht maximaal 30 seconden na verbinding |
| Telegram mislukt (401) | Ongeldig token | Controleer token via BotFather |
| Telegram mislukt (400) | Verkeerd chat-ID | Controleer chat-ID via @userinfobot |
| Touch reageert niet | I2C probleem | Controleer SDA/SCL, herstart |

---

## Debug aan/uit

In `src/ESP32C6_SmartDisplay.ino`:

```cpp
#define DEBUG_SERIAL 1   // 1 = debug aan, 0 = debug uit
#define DEBUG_LEVEL  3   // 0=uit, 1=errors, 2=warnings, 3=info, 4=verbose
```

Bij `DEBUG_SERIAL 0` is alle logging uitgeschakeld, ongeacht `DEBUG_LEVEL`.

---

## Library structuur

```
ESP32C6_SmartDisplay/
  src/
    ESP32C6_SmartDisplay.ino   <- Hoofdbestand
    debug_log.h                <- Logging macros
    config_manager.h           <- NVS configuratiebeheer
    lang.h                     <- Taalstrings (NL/EN)
    web_config.h               <- Webportaal (AP + STA)
    screen_datetime.h          <- Datum/Tijd scherm
    screen_phonefinder.h       <- Zoek Telefoon scherm
    screen_wifiscanner.h       <- WiFi Scanner scherm
    wifi_config_example.h      <- Voorbeeldbestand (NOOIT wifi_config.h committen)
  libraries/
    lv_conf.h                  <- LVGL configuratie
    Mylibrary/
      pin_config.h             <- Pin-definities
    lvgl/                      <- LVGL v9.5.0
    GFX_Library_for_Arduino/   <- GFX v1.5.9
    esp_lcd_touch_axs5106l/    <- Touch driver v1.0.0
  opdracht/
    opdracht.md                <- Functionele eisen
  documentatie/                <- Dit document
  platformio.ini               <- PlatformIO build-configuratie
  .gitignore
```

---

## Fail-safe gedrag

| Situatie | Gedrag |
|---------|--------|
| Display init mislukt | Foutlog, herstart na 5 seconden |
| LVGL buffer allocatie mislukt | Foutlog, herstart na 5 seconden |
| WiFi niet beschikbaar | Configuratieportaal als accesspoint |
| Telegram token/chat-ID in webportaal | Veld toont sterretjes als al opgeslagen. Leeg laten = huidige waarde behouden |
| NTP niet gesynchroniseerd | "--:--:--" tonen, elke seconde opnieuw proberen |
| Telegram token/chat-ID leeg | Foutmelding op scherm, geen HTTP-verzoek |
| WiFi verbroken tijdens Telegram | Foutmelding op scherm |

---

*Documentatie bijgehouden door JWP van Renen. Versie v2.4.0, 2026-03-06.*
