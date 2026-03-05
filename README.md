# ESP32C6 SmartDisplay

Multifunctioneel touchscreen dashboard voor de **Waveshare ESP32-C6-Touch-LCD-1.47**.

Drie interactieve schermen via LVGL v9:
- **Datum/Tijd** - NTP-gesynchroniseerde klok in het Nederlands, met zomertijd (Europe/Brussels)
- **Zoek Telefoon** - Verstuurt een Telegram-bericht om uw telefoon te laten rinkelen
- **WiFi Scanner** - Scant WiFi-netwerken met SSID, BSSID, kanaal, RSSI en beveiligingstype

Configuratie volledig via een **webportaal** in de browser: geen hercompileren vereist.

---

## Ondersteunde hardware

| Component | Specificatie |
|-----------|-------------|
| Bord | Waveshare ESP32-C6-Touch-LCD-1.47 |
| Chip | ESP32-C6FH8 (RISC-V, 160 MHz, 8 MB Flash) |
| Display | 1.47" IPS, 172x320 px, JD9853, SPI |
| Touch | AXS5106L, I2C (adres 0x63) |
| Connectiviteit | WiFi 6 (802.11ax), Bluetooth 5 BLE |

---

## Benodigde libraries

| Library | Versie | Locatie | Meegeleverd? |
|---------|--------|---------|-------------|
| lvgl | **9.5.0 (exact)** | `libraries/lvgl/` | Nee, zie installatie |
| GFX_Library_for_Arduino | **1.5.9 (exact)** | `libraries/GFX_Library_for_Arduino/` | Nee, zie installatie |
| esp_lcd_touch_axs5106l | 1.0.0 | `libraries/esp_lcd_touch_axs5106l/` | Ja |
| espressif32 (PlatformIO platform) | 3.0.0+ | PlatformIO registry | Via PlatformIO |

De custom touch driver is meegeleverd. De grote externe libraries (LVGL en GFX) moet
u handmatig installeren (zie installatiestappen hieronder).

> **Let op:** gebruik exact de opgegeven versies. Andere versies kunnen compilatiefouten veroorzaken.

---

## Build instructies (PlatformIO)

```bash
# 1. Clone het project
git clone https://github.com/MeLaJa/ESP32C6_SmartDisplay.git
cd ESP32C6_SmartDisplay

# 2. Installeer LVGL v9.5.0 in libraries/
#    Download van: https://github.com/lvgl/lvgl/releases/tag/v9.5.0
#    Pak uit naar: libraries/lvgl/

# 3. Installeer GFX_Library_for_Arduino v1.5.9 in libraries/
#    Download van: https://github.com/moononournation/Arduino_GFX/releases/tag/v1.5.9
#    Pak uit naar: libraries/GFX_Library_for_Arduino/

# 4. Kopieer het WiFi-voorbeeldbestand
cp src/wifi_config_example.h src/wifi_config.h

# 5. Compileer via PlatformIO CLI
pio run

# 6. Upload naar het bord (verbind USB-C eerst)
pio run --target upload
```

Of gebruik de **PlatformIO IDE** in VSCode: open de map, klik Build (vinkje) en Upload (pijltje).

> **Belangrijk:** de mappen `libraries/lvgl/` en `libraries/GFX_Library_for_Arduino/` zijn niet
> meegeleverd in de repository (te groot). Download ze handmatig via de links hierboven en
> plaats ze in de `libraries/` map.

---

## Upload instructies

1. Verbind de Waveshare ESP32-C6 via USB-C.
2. PlatformIO detecteert de poort automatisch.
3. Upload snelheid: **921600 baud**.
4. Na upload: open Serial Monitor op **115200 baud** voor debug-uitvoer.

---

## Configuratie

### Eerste opstart

Bij de eerste opstart (of als WiFi-verbinding mislukt) start het apparaat als accesspoint.

1. Verbind uw telefoon of laptop met WiFi **"ESP32-Config"** (geen wachtwoord).
2. Open **http://192.168.4.1** in een browser.
3. Vul in: WiFi SSID, wachtwoord, Telegram bot-token, chat-ID, tijdzone, taal.
4. Klik Opslaan. Het apparaat herstart en verbindt met uw WiFi.

### Na verbinding

Het configuratieportaal blijft bereikbaar via **http://[IP-adres]/** op uw eigen WiFi.
Het IP-adres wordt getoond op het Datum/Tijd scherm.

### Telegram instellen

1. Maak een bot aan via [@BotFather](https://t.me/BotFather) en kopieer het token.
2. Start een gesprek met uw bot en zoek uw chat-ID via [@userinfobot](https://t.me/userinfobot).
3. Vul token en chat-ID in via het configuratieportaal.

---

## Debug

In `src/ESP32C6_SmartDisplay.ino`:

```cpp
#define DEBUG_SERIAL 1   // 1 = aan, 0 = uit
#define DEBUG_LEVEL  3   // 0=uit, 1=errors, 2=warnings, 3=info, 4=verbose
```

---

## Projectstructuur

```
ESP32C6_SmartDisplay/
  src/
    ESP32C6_SmartDisplay.ino   <- Hoofdbestand
    debug_log.h                <- Logging
    config_manager.h           <- NVS configuratie
    lang.h                     <- Taalstrings (NL/EN)
    web_config.h               <- Webportaal
    screen_datetime.h          <- Datum/Tijd scherm
    screen_phonefinder.h       <- Zoek Telefoon scherm
    screen_wifiscanner.h       <- WiFi Scanner scherm
    wifi_config_example.h      <- Voorbeeldbestand (kopieer naar wifi_config.h)
  libraries/
    lv_conf.h
    Mylibrary/pin_config.h
    lvgl/
    GFX_Library_for_Arduino/
    esp_lcd_touch_axs5106l/
  documentatie/
  opdracht/
  platformio.ini
  .gitignore
```

---

## Disclaimer

Dit project is gepubliceerd voor educatieve doeleinden en persoonlijk gebruik.
Het SSL-certificaat van de Telegram API wordt niet geverifieerd (acceptabel voor privé IoT).
Gebruik in productieomgevingen op eigen verantwoordelijkheid.

---

*Versie v2.0.0. Ontwikkeld door JWP van Renen, met ondersteuning van Co-worker.*
