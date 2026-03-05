# CHANGELOG.md

**Project:** ESP32C6_SMARTDISPLAY
**Auteur:** JWP van Renen

---

## v2.0.0 - 2026-03-05

### Nieuw
- Configuratieportaal via webinterface (AP-modus en STA-modus).
  Bij eerste opstart of mislukte WiFi-verbinding start het apparaat als "ESP32-Config" accesspoint.
  Alle instellingen (WiFi, Telegram, tijdzone, taal) zijn via de browser aanpasbaar zonder hercompileren.
- Tweetalige interface (Nederlands/Engels) via `lang.h`. Taal instelbaar via webportaal.
- Zoek Telefoon scherm verstuurt Telegram-bericht via HTTPS (WiFiClientSecure).
  Configuratie van token en chat-ID volledig via webportaal, geen hardcoded credentials meer.
- Captive portal DNS: alle DNS-verzoeken in AP-modus worden omgeleid naar 192.168.4.1.
- WebServer in STA-modus: portaal bereikbaar via http://[IP]/ na WiFi-verbinding.
- IP-adres getoond op het Datum/Tijd scherm.
- `config_manager.h` voor NVS-opslag van alle instellingen.
- `web_config.h` met volledig tweetalig HTML-formulier.

### Gewijzigd
- Versiebeheer: MAJOR-versie verhoogd naar v2.0.0 (breaking: WiFi config verplaatst naar NVS).
- `wifi_config.h` is niet meer de primaire configuratiebron. NVS heeft prioriteit.
- `screen_phonefinder.h` herschreven: BLE-scan vervangen door Telegram-bericht.

### Verwijderd
- BLE-scan voor telefoonzoeker (vervangen door Telegram HTTPS methode).

---

## v1.0.0 - 2026-03-04

### Initieel
- Datum/Tijd scherm met NTP-synchronisatie (tijdzone Europe/Brussels).
- Telefoon Zoeker scherm met BLE-scan en RSSI-weergave.
- WiFi Scanner scherm met netwerkenlijst en detailpaneel.
- LVGL v9 tabview met drie tabbladen (home, bel, wifi iconen).
- Display initialisatie via GFX_Library_for_Arduino (JD9853 registers).
- Touch via AXS5106L (I2C, adres 0x63).
- Debug logging via `debug_log.h` (DEBUG_SERIAL + DEBUG_LEVEL).
- Lokale libraries structuur (lvgl 9.5.0, GFX 1.5.9, touch 1.0.0).
- PlatformIO configuratie voor ESP32-C6 (160 MHz, 8 MB Flash).

---

*CHANGELOG bijgehouden met ondersteuning van Co-worker.*
