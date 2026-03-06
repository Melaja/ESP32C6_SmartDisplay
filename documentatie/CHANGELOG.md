# CHANGELOG.md

**Project:** ESP32C6_SMARTDISPLAY
**Auteur:** JWP van Renen

---

## v2.6.0 - 2026-03-06

### Nieuw
- Webportaal: WiFi-netwerken scannen via knop "Scan" naast het SSID-veld.
  ESP32 scant beschikbare netwerken en toont ze in een dropdown; klik om te selecteren.
  Werkt zowel in AP-modus (eerste configuratie) als STA-modus.
- Webportaal: sectie "Schermverlichting" toegevoegd.
  Dimmen en uitschakelen instellen in seconden (0 = uitgeschakeld).
- Scherm dimt automatisch na ingestelde tijd zonder aanraking.
- Scherm schakelt uit na tweede ingestelde tijd.
- Aanraking wekt het scherm op; eerste aanraking wordt doorgeslikt zodat geen onbedoelde UI-actie plaatsvindt.
- `config_manager.h`: velden `dim_vertraging` en `uit_vertraging` (uint16_t seconden), NVS-sleutels `dim_sec` en `uit_sec`.
- `web_config.h`: `/scan` endpoint (JSON) + JavaScript scan-UI + dim/uit-invoervelden.
- `ESP32C6_SmartDisplay.ino`: `bl_instellen()` hulpfunctie, backlight-timer in `loop()`, aanraakdetectie bijgewerkt.

---

## v2.5.0 - 2026-03-06

### Nieuw
- Splash screen getoond bij opstart gedurende 5 seconden.
  Afbeelding: 172x320 px, RGB565, opgeslagen in PROGMEM (110 KB in Flash).
- `screen_splash.h`: bevat de afbeeldingsdata en `splash_tonen(gfx)` functie.
- `ESP32C6_SmartDisplay.ino`: `splash_tonen(gfx)` aangeroepen tussen
  `display_initialiseren()` en `lvgl_initialiseren()` in de setup().

---

## v2.4.0 - 2026-03-06

### Nieuw
- Webportaal: veld "Beltekst" toegevoegd in de CallMeBot-sectie.
  De gesproken tekst bij het bellen is nu instelbaar via het configuratiescherm (standaard: "Zoek mijn telefoon").
- `config_manager.h`: veld `callmebot_tekst[128]` toegevoegd, opgeslagen als NVS-sleutel `cb_tekst`.
- `screen_phonefinder.h`: tekst wordt URL-gecodeerd via `pf_url_encode()` voor gebruik in de CallMeBot-URL.

---

## v2.3.0 - 2026-03-06

### Gewijzigd
- Zoek Telefoon scherm gebruikt nu CallMeBot in plaats van een Telegram-tekstbericht.
  De ESP32 belt uw telefoon via tekst-naar-spraak (CallMeBot API) zodat u het kunt horen.
- `config_manager.h`: Telegram token/chat-ID velden vervangen door `callmebot_user`.
- `web_config.h`: Telegram-sectie vervangen door CallMeBot-sectie met setup-instructies.
- `screen_phonefinder.h`: HTTP GET naar `api2.callmebot.com` met SSID, taal `nl-NL-Standard-B`, 2x herhalen.
- `lang.h`: phonefinder teksten bijgewerkt (knop "Bellen", status "Oproep gestart!").

---

## v2.2.0 - 2026-03-04

### Fix
- Webportaal POST-handler: SSID, Bot Token en Chat ID worden nu getrimd (spaties en regelafbrekingen verwijderd).
  Voorkomt HTTP 400/401-fouten bij Telegram als de gebruiker de waarde kopieert uit een Telegram-bericht.

---

## v2.1.0 - 2026-03-05

### Beveiliging
- Telegram Bot Token en Chat ID worden niet meer als leesbare tekst getoond in het webportaal.
  Als een waarde al opgeslagen is, verschijnt een afgeschermd invoerveld (type password) met
  de melding "Opgeslagen - laat leeg om te behouden". De echte waarde verlaat nooit de NVS.
- POST-handler: een leeg ingestuurd token of chat-ID-veld behoudt de bestaande opgeslagen waarde.
  Zo kan de gebruiker andere instellingen aanpassen zonder het token opnieuw in te voeren.

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

*CHANGELOG bijgehouden door JWP van Renen.*
