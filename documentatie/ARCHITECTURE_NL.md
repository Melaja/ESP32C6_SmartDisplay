# ARCHITECTURE_NL.md - Architectuurbeschrijving (NL)

**Project:** ESP32C6_SMARTDISPLAY
**Versie:** v2.0.0
**Auteur:** JWP van Renen
**Datum:** 2026-03-05

---

## Overzicht

Het project bestaat uit een enkel Arduino .ino bestand met meerdere header-modules.
Elke module is verantwoordelijk voor een afgebakend onderdeel van de functionaliteit.
De keuze voor header-only modules (.h) is bewust. Het vermijdt compile-volgorde problemen
in de Arduino/PlatformIO omgeving en houdt elke module zelfvoorzienend.

---

## Systeemarchitectuur (ASCII)

```
+--------------------------------------------------------------+
|                   ESP32C6_SmartDisplay.ino                   |
|  (setup + loop, initialisatie, tabblad-routing)              |
+------+----------+----------+----------+---------------------+
       |          |          |          |
       v          v          v          v
  debug_log.h  lang.h  config_manager.h  web_config.h
  (logging)  (talen)   (NVS opslag)    (webportaal AP+STA)
       |
       +--------------------+-------------------+
       |                    |                   |
  screen_datetime.h  screen_phonefinder.h  screen_wifiscanner.h
  (Datum/Tijd)       (Zoek Telefoon)       (WiFi Scanner)
       |                    |                   |
    LVGL v9             Telegram API        WiFi scan API
    NTP/time.h          WiFiClientSecure    esp-idf WiFi
```

---

## Module beschrijvingen

### ESP32C6_SmartDisplay.ino

Hoofdbestand. Verantwoordelijk voor:
- Initialisatie van display (GFX + JD9853 registers), LVGL, touch (AXS5106L), BLE en WiFi
- Laden van configuratie uit NVS via `config_manager.h`
- WiFi-verbinding of start van het configuratieportaal
- Opbouw van de LVGL tabview met drie tabbladen
- Arduino `loop()`: LVGL task handler en webserver verzoeken

### debug_log.h

Centrale logging module. Alle Serial Monitor uitvoer verloopt uitsluitend via de macros
`DBG_ERROR`, `DBG_WARN`, `DBG_INFO` en `DBG_VERBOSE`. De schakelaar `DEBUG_SERIAL` en
`DEBUG_LEVEL` worden gedefinieerd in het hoofd-.ino bestand.

### config_manager.h

Configuratiebeheer via ESP32 NVS (Non-Volatile Storage, Preferences API).
Slaat WiFi-gegevens, Telegram-instellingen, tijdzone en taal op in interne flash.
Bevat `config_laden()`, `config_opslaan()` en `config_wissen()`.

### lang.h

Meertalige tekst-strings (Nederlands/Engels) via een `Lang`-struct.
De globale pointer `g_lang` verwijst naar de actieve taal. Alle zichtbare
teksten in de schermen worden via `g_lang->sleutel` opgehaald.

### web_config.h

Tweetalig (NL/EN) configuratieportaal via HTTP.
- **AP-modus:** start "ESP32-Config" accesspoint met captive portal DNS.
  Alle DNS-verzoeken worden omgeleid naar 192.168.4.1.
- **STA-modus:** WebServer bereikbaar via het eigen WiFi-IP-adres.
  Formulier toont huidige config-waarden. Na opslaan herstart het apparaat.

### screen_datetime.h

Datum/Tijd scherm. Werkt via een LVGL timer (1 seconde interval) die `localtime_r()`
aanroept om de systeemtijd (gesynchroniseerd via NTP) te lezen en de labels bij te werken.
Toont ook het IP-adres en de NTP-synchronisatiestatus.

### screen_phonefinder.h

Zoek Telefoon scherm. Verstuurt een HTTPS GET-verzoek naar de Telegram Bot API
(`api.telegram.org`) via `WiFiClientSecure`. SSL-verificatie is uitgeschakeld
(acceptabel voor privé IoT-gebruik). Een dubbel-klik-beveiliging (`pf_bezig` vlag)
voorkomt meerdere gelijktijdige HTTP-verzoeken.

### screen_wifiscanner.h

WiFi Scanner scherm. Voert een asynchrone WiFi-scan uit via `WiFi.scanNetworks(true)`.
Een LVGL poll-timer (500 ms) controleert of de scan klaar is. Dit blokkeert de LVGL UI niet.
Toont resultaten in een scrollbare lijst. Klik op een netwerk opent een detailpaneel met
SSID, BSSID, kanaal, frequentie, RSSI en beveiligingstype.

---

## Taalmodule

```
lang.h
  +-- struct Lang { ... }   <- alle tekst-sleutels
  +-- const Lang LANG_NL    <- Nederlandse teksten
  +-- const Lang LANG_EN    <- Engelse teksten

extern const Lang* g_lang;  <- actieve taalpointer (ingesteld in .ino)
```

---

## Configuratiestroom

```
[boot]
  |
  v
config_laden() uit NVS
  |
  +-- geconfigureerd == false of SSID leeg?
  |     -> webconfig_ap_starten()  [eindeloze loop + reboot na opslaan]
  |
  +-- geconfigureerd == true
        -> wifi_verbinden()
        |
        +-- mislukt? -> webconfig_ap_starten()
        |
        +-- geslaagd?
              -> NTP starten
              -> BLE initialiseren
              -> webconfig_sta_starten()
              -> ui_aanmaken()
              -> loop()
```

---

## Display initialisatie

De JD9853 displaycontroller vereist een specifieke reeks SPI-registercommando's
bij opstart. Deze volgorde is vastgelegd in `display_register_init()` als een
byte-array die via `batchOperation()` naar de displaybus gestuurd wordt.
De `Arduino_ST7789`-driver van GFX_Library_for_Arduino wordt gebruikt als
basisklasse (compatibel qua SPI-protocol).

---

## LVGL integratie

- **Buffer:** Enkelbuffer van `LCD_BREEDTE * 40` pixels in intern SRAM.
  Fallback naar extern geheugen als intern allocatie mislukt.
- **Flush callback:** `lvgl_flush_cb()` roept `gfx->draw16bitRGBBitmap()` aan.
- **Tick callback:** `lvgl_tick_cb()` retourneert `millis()`.
- **Touch:** `lvgl_touch_read_cb()` leest coördinaten via `bsp_touch_read()` (AXS5106L driver).
- **Tabview:** Drie tabbladen onderaan (42 px tabbalk) met LVGL-symbolen als iconen.

---

## Pin-configuratie

Alle pin-definities staan uitsluitend in `libraries/Mylibrary/pin_config.h`.
Geen hardcoded pin-nummers in het hoofd-.ino bestand of in de schermmodules.

---

## Geheugengebruik (bij v2.0.0)

- **Flash:** circa 1.6 MB van 8 MB (GFX + LVGL + WiFi + BLE)
- **SRAM heap na setup:** circa 180-220 KB vrij (afhankelijk van BLE-status)
- **LVGL buffer:** 172 * 40 * 2 bytes = circa 13.8 KB

---

*Architectuurdocumentatie gegenereerd met ondersteuning van Co-worker. Versie v2.0.0, 2026-03-05.*
