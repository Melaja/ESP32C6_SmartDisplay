# TRACEABILITY_MATRIX.md

**Project:** ESP32C6_SMARTDISPLAY
**Versie:** v2.0.0
**Auteur:** JWP van Renen
**Datum:** 2026-03-05

---

## Traceerbaarheidsmatrix

| Eis ID | Beschrijving | Bron (opdracht.md sectie) | Codebestand | Functie/Module | Status |
|--------|-------------|--------------------------|------------|----------------|--------|
| REQ-001 | Toont huidige datum in Nederlands formaat ("07 mei 1955 zaterdag") | Functionele Eisen, 1. Datum/Tijd | screen_datetime.h | `dt_update_weergave()`, `g_lang->maanden[]`, `g_lang->dagen[]` | Geimplementeerd |
| REQ-002 | Toont huidige tijd in 24-uurs formaat ("23:59:59") | Functionele Eisen, 1. Datum/Tijd | screen_datetime.h | `dt_update_weergave()`, `snprintf(tijdbuf)` | Geimplementeerd |
| REQ-003 | Tijd gesynchroniseerd via NTP over WiFi, automatische zomertijd (Europe/Brussels) | Functionele Eisen, 1. Datum/Tijd | ESP32C6_SmartDisplay.ino, config_manager.h | `wifi_verbinden()`, `configTzTime()`, `g_config.ntp_tijdzone` | Geimplementeerd |
| REQ-004 | Zoek Telefoon scherm aanwezig | Functionele Eisen, 2. Telefoon Zoeker | screen_phonefinder.h | `scherm_phonefinder_aanmaken()` | Geimplementeerd |
| REQ-005 | Verstuurt Telegram-bericht bij drukken op knop | Functionele Eisen, 2. Telefoon Zoeker | screen_phonefinder.h | `pf_bel_callback()`, `http.GET()` | Geimplementeerd |
| REQ-006 | Configuratie via webportaal (token, chat-ID) | Functionele Eisen, 2. Telefoon Zoeker | web_config.h, config_manager.h | `webconfig_ap_starten()`, `config_opslaan()` | Geimplementeerd |
| REQ-007 | Foutmelding bij ontbrekende configuratie of mislukte verbinding | Functionele Eisen, 2. Telefoon Zoeker | screen_phonefinder.h | `pf_bel_callback()` (WiFi check, token check, HTTP code check) | Geimplementeerd |
| REQ-008 | WiFi Scanner scherm aanwezig | Functionele Eisen, 3. WiFi Scanner | screen_wifiscanner.h | `scherm_wifiscanner_aanmaken()` | Geimplementeerd |
| REQ-009 | Scant beschikbare WiFi-netwerken | Functionele Eisen, 3. WiFi Scanner | screen_wifiscanner.h | `ws_scan_callback()`, `WiFi.scanNetworks(true)` | Geimplementeerd |
| REQ-010 | Lijst van gevonden netwerken | Functionele Eisen, 3. WiFi Scanner | screen_wifiscanner.h | `ws_update_lijst()`, `ws_maak_netwerk_item()` | Geimplementeerd |
| REQ-011 | Detailpagina na klik op SSID (SSID, BSSID, kanaal, frequentie, RSSI, beveiliging) | Functionele Eisen, 3. WiFi Scanner | screen_wifiscanner.h | `ws_toon_detail()` | Geimplementeerd |
| REQ-012 | Kleurgecodeerde signaalsterkte in lijst | Functionele Eisen, 3. WiFi Scanner | screen_wifiscanner.h | `ws_rssi_naar_kleur()`, `ws_maak_netwerk_item()` | Geimplementeerd |
| REQ-013 | Alle code in LVGL v9 (laatste versie) | Niet-Functionele Eisen, Technisch | Alle schermbestanden | lv_* functies | Geimplementeerd |
| REQ-014 | Uitgebreid Nederlands commentaar | Niet-Functionele Eisen, Technisch | Alle .ino en .h bestanden | Commentaar in alle modules | Geimplementeerd |
| REQ-015 | Schakelbare debug output (DEBUG_SERIAL) | Niet-Functionele Eisen, Technisch | ESP32C6_SmartDisplay.ino, debug_log.h | `#define DEBUG_SERIAL`, `DBG_*` macros | Geimplementeerd |
| REQ-016 | Lokale libraries in /libraries/ | Niet-Functionele Eisen, Technisch | platformio.ini | `lib_dir = libraries` | Geimplementeerd |
| REQ-017 | Geen magische getallen | Niet-Functionele Eisen, Kwaliteit | Alle bestanden | Named constants (WIFI_MAX_POGINGEN, BL_HELDERHEID, etc.) | Geimplementeerd |
| REQ-018 | Fail-safe gedrag voor WiFi, BLE, display | Niet-Functionele Eisen, Kwaliteit | ESP32C6_SmartDisplay.ino, screen_phonefinder.h | `display_initialiseren()`, `wifi_verbinden()`, `pf_bel_callback()` | Geimplementeerd |
| REQ-019 | Versie v1.0.0 bij boot gelogd | Niet-Functionele Eisen, Kwaliteit | ESP32C6_SmartDisplay.ino, debug_log.h | `dbg_boot_info(VERSIE_STRING)` | Geimplementeerd |

---

## Statuslegenda

| Status | Betekenis |
|--------|----------|
| Ontwerp | Eis geanalyseerd, implementatie gepland |
| Geimplementeerd | Code aanwezig en functioneel |
| Getest | Getest op hardware en goedgekeurd |
| Verworpen | Eis buiten scope geplaatst |

---

*Traceerbaarheidsmatrix bijgehouden met ondersteuning van Co-worker. Versie v2.0.0, 2026-03-05.*
