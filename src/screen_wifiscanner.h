/*
  Bestand : screen_wifiscanner.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : WiFi Netwerk Scanner scherm voor ESP32C6_SmartDisplay.
            Scant beschikbare WiFi-netwerken in de omgeving en toont
            gedetailleerde informatie per netwerk:
            - SSID (netwerknaam)
            - BSSID (MAC-adres van het access point)
            - Kanaal (1-14 voor 2.4GHz, 36+ voor 5GHz)
            - Signaalsterkte (RSSI in dBm + visuele balk)
            - Beveiligingstype (Open, WEP, WPA/WPA2, WPA3, etc.)
            - Netwerk type op basis van WiFi-standaard

            De ESP32-C6 ondersteunt WiFi 6 (802.11ax) op 2.4GHz.
            De scan kan worden uitgevoerd terwijl de WiFi-verbinding actief is
            (WiFi station modus + scan tegelijk, ingebouwd in ESP32 WiFi-stack).
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <lvgl.h>
#include <WiFi.h>
#include "debug_log.h"
#include "lang.h"

// ============================================================
// CONSTANTEN
// ============================================================

// Maximaal aantal netwerken in de lijst (geheugenbeperking)
static const uint8_t WIFI_MAX_NETWERKEN = 25;

// ============================================================
// SCHERM STATUS
// ============================================================
static bool ws_scan_bezig        = false;
static int  ws_geselecteerd_idx  = -1;   // Index van geselecteerd netwerk

// ============================================================
// LVGL WIDGET POINTERS
// ============================================================
static lv_obj_t*   ws_netwerk_lijst   = NULL;   // Lijst van gevonden netwerken
static lv_obj_t*   ws_detail_panel    = NULL;   // Detailpaneel voor geselecteerd netwerk
static lv_obj_t*   ws_btn_scan        = NULL;   // Scan knop
static lv_obj_t*   ws_btn_terug       = NULL;   // Terug knop (van detail naar lijst)
static lv_obj_t*   ws_label_status    = NULL;   // Status tekst
static lv_obj_t*   ws_spinner         = NULL;   // Laad-animatie
static lv_obj_t*   ws_detail_inhoud   = NULL;   // Tekstlabel in detailpaneel
static lv_timer_t* ws_poll_timer      = NULL;   // Polling-timer voor async WiFi scan

// ============================================================
// HULPFUNCTIE: BEVEILIGINGSTYPE NAAR TEKST
// Converteert Arduino WiFi encryptie-constante naar leesbare string
// ============================================================
static const char* ws_beveiliging_tekst(wifi_auth_mode_t type) {
  switch (type) {
    case WIFI_AUTH_OPEN:            return g_lang->sec_open;
    case WIFI_AUTH_WEP:             return g_lang->sec_wep;
    case WIFI_AUTH_WPA_PSK:         return "WPA-PSK";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2-PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2-PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3-PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3-PSK";
    case WIFI_AUTH_WAPI_PSK:        return "WAPI-PSK";
    default:                         return g_lang->sec_onbekend;
  }
}

// ============================================================
// HULPFUNCTIE: RSSI NAAR SIGNAALBESCHRIJVING
// ============================================================
static const char* ws_rssi_beschrijving(int32_t rssi) {
  if (rssi >= -50) return g_lang->sig_uitstekend;
  if (rssi >= -60) return g_lang->sig_goed;
  if (rssi >= -70) return g_lang->sig_redelijk;
  if (rssi >= -80) return g_lang->sig_zwak;
  return g_lang->sig_zeer_zwak;
}

// ============================================================
// HULPFUNCTIE: RSSI NAAR KLEUR
// ============================================================
static lv_color_t ws_rssi_naar_kleur(int32_t rssi) {
  if (rssi >= -60) return lv_color_hex(0x44DD44);  // Groen
  if (rssi >= -70) return lv_color_hex(0xDDDD00);  // Geel
  if (rssi >= -80) return lv_color_hex(0xFF8800);  // Oranje
  return lv_color_hex(0xDD3333);                    // Rood
}

// ============================================================
// HULPFUNCTIE: RSSI NAAR PERCENTAGE (voor balk)
// RSSI range -100 tot -30 dBm → 0% tot 100%
// ============================================================
static int32_t ws_rssi_naar_pct(int32_t rssi) {
  int32_t pct = (rssi + 100) * 100 / 70;
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return pct;
}

// ============================================================
// DETAILPANEEL: Toon gedetailleerde info voor geselecteerd netwerk
// ============================================================
static void ws_toon_detail(int idx) {
  if (idx < 0 || idx >= WiFi.scanComplete()) return;
  if (ws_detail_inhoud == NULL) return;

  // Alle beschikbare informatie voor het geselecteerde netwerk
  String ssid = WiFi.SSID(idx);
  if (ssid.length() == 0) ssid = g_lang->wifi_verborgen;

  // Signaalbeschrijving
  int32_t rssi = WiFi.RSSI(idx);
  const char* sterkte_tekst = ws_rssi_beschrijving(rssi);

  // Beveiligingstype
  wifi_auth_mode_t encryptie = WiFi.encryptionType(idx);
  const char* bev_tekst = ws_beveiliging_tekst(encryptie);

  // Kanaalinformatie
  int32_t kanaal = WiFi.channel(idx);

  // BSSID (MAC-adres van het access point)
  uint8_t* bssid = WiFi.BSSID(idx);
  char bssid_tekst[18];
  snprintf(bssid_tekst, sizeof(bssid_tekst),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           bssid[0], bssid[1], bssid[2],
           bssid[3], bssid[4], bssid[5]);

  // Kanaalfrequentie berekenen:
  // 2.4GHz band: kanaal 1-13 → 2412 + (kanaal-1)*5 MHz
  int32_t freq_mhz = 0;
  if (kanaal >= 1 && kanaal <= 13) {
    freq_mhz = 2412 + (kanaal - 1) * 5;
  } else if (kanaal == 14) {
    freq_mhz = 2484;  // Kanaal 14 is speciaal (Japan)
  } else if (kanaal >= 36) {
    // 5GHz band (toekomstige uitbreiding)
    freq_mhz = 5000 + kanaal * 5;
  }

  // Detail tekst opbouwen
  char detail[400];
  snprintf(detail, sizeof(detail),
    g_lang->wifi_detail_fmt,
    ssid.c_str(),
    bssid_tekst,
    rssi, sterkte_tekst,
    kanaal, freq_mhz,
    bev_tekst
  );

  lv_label_set_text(ws_detail_inhoud, detail);

  // Toon detailpaneel, verberg lijst
  lv_obj_clear_flag(ws_detail_panel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ws_netwerk_lijst, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ws_btn_terug, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ws_btn_scan, LV_OBJ_FLAG_HIDDEN);

  lv_label_set_text(ws_label_status, ssid.c_str());

  DBG_INFO("Netwerk detail getoond: %s (RSSI=%ld, kanaal=%ld)",
           ssid.c_str(), rssi, kanaal);
}

// ============================================================
// TERUG KNOP: Terug naar de netwerkenlijst
// ============================================================
static void ws_terug_callback(lv_event_t* e) {
  LV_UNUSED(e);
  // Verberg detail, toon lijst
  lv_obj_add_flag(ws_detail_panel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ws_netwerk_lijst, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(ws_btn_terug, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ws_btn_scan, LV_OBJ_FLAG_HIDDEN);

  // Status herstellen naar aantal gevonden netwerken
  int n = WiFi.scanComplete();
  if (n > 0) {
    char buf[48];
    snprintf(buf, sizeof(buf), "%d %s", n, n == 1 ? g_lang->wifi_netw_enkv : g_lang->wifi_netw_mv);
    lv_label_set_text(ws_label_status, buf);
  } else {
    lv_label_set_text(ws_label_status, g_lang->wifi_druk_scan);
  }
  DBG_VERBOSE("Terug naar netwerkenlijst.");
}

// ============================================================
// NETWERK ITEM AANMAKEN IN DE LIJST
// ============================================================
static void ws_maak_netwerk_item(lv_obj_t* parent, int idx) {
  String ssid = WiFi.SSID(idx);
  if (ssid.length() == 0) ssid = g_lang->wifi_verborgen;
  int32_t rssi = WiFi.RSSI(idx);

  // Container rij: vaste hoogte 40px zodat BOTTOM-uitlijning correct werkt.
  // LV_SIZE_CONTENT werkt niet goed met ALIGN_BOTTOM in LVGL v9 (hoogte = 0 bij berekening).
  lv_obj_t* rij = lv_obj_create(parent);
  lv_obj_set_size(rij, lv_pct(100), 40);
  lv_obj_set_style_bg_color(rij, lv_color_hex(0x1A1A3A), 0);
  lv_obj_set_style_bg_color(rij, lv_color_hex(0x3A3A6A), LV_STATE_PRESSED);
  lv_obj_set_style_border_color(rij, lv_color_hex(0x333366), 0);
  lv_obj_set_style_border_width(rij, 1, 0);
  lv_obj_set_style_radius(rij, 4, 0);
  lv_obj_set_style_pad_all(rij, 4, 0);
  lv_obj_clear_flag(rij, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(rij, LV_OBJ_FLAG_CLICKABLE);

  // Sla de netwerk-index op als gebruikersdata voor de callback
  lv_obj_set_user_data(rij, (void*)(intptr_t)idx);

  // Klik-callback: open detailpaneel
  lv_obj_add_event_cb(rij, [](lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target_obj(e);
    int idx = (int)(intptr_t)lv_obj_get_user_data(obj);
    ws_toon_detail(idx);
  }, LV_EVENT_CLICKED, NULL);

  // SSID naam (bovenste rij, links) — max 88px breed zodat RSSI rechts past
  lv_obj_t* ssid_lbl = lv_label_create(rij);
  char ssid_buf[34];
  strncpy(ssid_buf, ssid.c_str(), 33);
  ssid_buf[33] = '\0';
  lv_label_set_text(ssid_lbl, ssid_buf);
  lv_obj_set_style_text_font(ssid_lbl, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(ssid_lbl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(ssid_lbl, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_label_set_long_mode(ssid_lbl, LV_LABEL_LONG_CLIP);
  lv_obj_set_width(ssid_lbl, 88);

  // RSSI waarde (bovenste rij, rechts) — kleurgecodeerd
  char rssi_buf[10];
  snprintf(rssi_buf, sizeof(rssi_buf), "%ld dBm", rssi);
  lv_obj_t* rssi_lbl = lv_label_create(rij);
  lv_label_set_text(rssi_lbl, rssi_buf);
  lv_obj_set_style_text_font(rssi_lbl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(rssi_lbl, ws_rssi_naar_kleur(rssi), 0);
  lv_obj_align(rssi_lbl, LV_ALIGN_TOP_RIGHT, 0, 1);

  // Kanaal + slot-icoon (onderste rij, links)
  char info_buf[20];
  snprintf(info_buf, sizeof(info_buf), "Ch %ld  %s",
           (long)WiFi.channel(idx),
           WiFi.encryptionType(idx) == WIFI_AUTH_OPEN ? LV_SYMBOL_EYE_OPEN : LV_SYMBOL_CLOSE);
  lv_obj_t* info_lbl = lv_label_create(rij);
  lv_label_set_text(info_lbl, info_buf);
  lv_obj_set_style_text_font(info_lbl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(info_lbl, lv_color_hex(0x888899), 0);
  lv_obj_align(info_lbl, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  // Signaalbalk (onderste rij, rechts) — 48px breed, los van RSSI-tekst erboven
  lv_obj_t* balk = lv_bar_create(rij);
  lv_obj_set_size(balk, 48, 6);
  lv_obj_align(balk, LV_ALIGN_BOTTOM_RIGHT, 0, -2);
  lv_bar_set_range(balk, 0, 100);
  lv_bar_set_value(balk, ws_rssi_naar_pct(rssi), LV_ANIM_OFF);
  lv_obj_set_style_bg_color(balk, lv_color_hex(0x333344), LV_PART_MAIN);
  lv_obj_set_style_bg_color(balk, ws_rssi_naar_kleur(rssi), LV_PART_INDICATOR);
}

// ============================================================
// LIJST BIJWERKEN NA SCAN
// ============================================================
static void ws_update_lijst(void) {
  if (ws_netwerk_lijst == NULL) return;
  lv_obj_clean(ws_netwerk_lijst);

  int n = WiFi.scanComplete();
  if (n <= 0) {
    lv_obj_t* lbl = lv_label_create(ws_netwerk_lijst);
    lv_label_set_text(lbl, n == 0 ? g_lang->wifi_geen_netwerken : g_lang->wifi_scan_mislukt);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_center(lbl);
    return;
  }

  for (int i = 0; i < n && i < WIFI_MAX_NETWERKEN; i++) {
    ws_maak_netwerk_item(ws_netwerk_lijst, i);
  }

  DBG_INFO("WiFi lijst bijgewerkt: %d netwerken weergegeven (van %d gevonden)",
           n < WIFI_MAX_NETWERKEN ? n : WIFI_MAX_NETWERKEN, n);
}

// ============================================================
// SCAN KLAAR: UI bijwerken met resultaten
// ============================================================
static void ws_scan_klaar(int n) {
  ws_scan_bezig = false;
  lv_obj_clear_state(ws_btn_scan, LV_STATE_DISABLED);
  lv_obj_add_flag(ws_spinner, LV_OBJ_FLAG_HIDDEN);

  char status_buf[40];
  if (n < 0) {
    snprintf(status_buf, sizeof(status_buf), "%s", g_lang->wifi_scan_mislukt);
    DBG_ERROR("WiFi scan mislukt: %d", n);
  } else {
    snprintf(status_buf, sizeof(status_buf), "%d %s",
             n, n == 1 ? g_lang->wifi_netw_enkv : g_lang->wifi_netw_mv);
    DBG_INFO("WiFi scan klaar: %d netwerken", n);
  }
  lv_label_set_text(ws_label_status, status_buf);
  ws_update_lijst();
}

// ============================================================
// POLL TIMER CALLBACK: controleert elke 500ms of async scan klaar is
// ============================================================
static void ws_scan_poll_cb(lv_timer_t* timer) {
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_RUNNING) return;  // nog bezig, volgende poll

  // Scan klaar: timer opruimen en resultaten tonen
  lv_timer_del(timer);
  ws_poll_timer = NULL;
  ws_scan_klaar(n);
}

// ============================================================
// SCAN KNOP CALLBACK
// Gebruikt async scan (niet-blokkerend) zodat de LVGL UI
// blijft reageren en de pressed-kleur zichtbaar is.
// ============================================================
static void ws_scan_callback(lv_event_t* e) {
  LV_UNUSED(e);
  if (ws_scan_bezig) {
    DBG_WARN("WiFi scan al bezig.");
    return;
  }

  DBG_INFO("WiFi scan gestart (async)...");
  ws_scan_bezig = true;

  // UI aanpassen VOOR de scan (zichtbaar door niet-blokkerende aanpak)
  lv_obj_add_state(ws_btn_scan, LV_STATE_DISABLED);
  lv_label_set_text(ws_label_status, g_lang->wifi_scannen);
  lv_obj_clear_flag(ws_spinner, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clean(ws_netwerk_lijst);

  // Vorige scanresultaten wissen zodat nieuwe scan correct kan starten
  WiFi.scanDelete();

  // Async scan starten: keert direct terug, blokkeert LVGL NIET
  WiFi.scanNetworks(true);

  // Poll elke 500ms of de scan klaar is
  ws_poll_timer = lv_timer_create(ws_scan_poll_cb, 500, NULL);
}

// ============================================================
// SCREEN AANMAKEN
// ============================================================
static void scherm_wifiscanner_aanmaken(lv_obj_t* parent) {
  DBG_INFO("WiFi Scanner scherm aanmaken...");

  // Achtergrond: donkerblauw/turquoise tint
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x0A1A2E), 0);
  lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

  // --- Titellabel ---
  lv_obj_t* titel = lv_label_create(parent);
  lv_label_set_text(titel, LV_SYMBOL_WIFI " WiFi Scanner");
  lv_obj_set_style_text_font(titel, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(titel, lv_color_hex(0x44AAFF), 0);
  lv_obj_align(titel, LV_ALIGN_TOP_MID, 0, 15);  // y=15 ipv 5: ronde hoeken display

  // --- Status label ---
  ws_label_status = lv_label_create(parent);
  lv_label_set_text(ws_label_status, g_lang->wifi_druk_scan);
  lv_obj_set_style_text_font(ws_label_status, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(ws_label_status, lv_color_hex(0x88AACC), 0);
  lv_obj_align(ws_label_status, LV_ALIGN_TOP_MID, 0, 38);
  lv_label_set_long_mode(ws_label_status, LV_LABEL_LONG_WRAP);  // wrap ipv clip
  lv_obj_set_width(ws_label_status, 152);
  lv_obj_set_style_text_align(ws_label_status, LV_TEXT_ALIGN_CENTER, 0);

  // --- Scan knop ---
  ws_btn_scan = lv_btn_create(parent);
  lv_obj_set_size(ws_btn_scan, 140, 32);
  lv_obj_align(ws_btn_scan, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_obj_set_style_bg_color(ws_btn_scan, lv_color_hex(0x1A4466), 0);
  lv_obj_set_style_bg_color(ws_btn_scan, lv_color_hex(0x2E6699), LV_STATE_PRESSED);  // iets lichter blauw
  lv_obj_add_event_cb(ws_btn_scan, ws_scan_callback, LV_EVENT_CLICKED, NULL);

  lv_obj_t* scan_lbl = lv_label_create(ws_btn_scan);
  lv_label_set_text(scan_lbl, LV_SYMBOL_REFRESH " Scan");
  lv_obj_set_style_text_font(scan_lbl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(scan_lbl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(scan_lbl);

  // --- Terug knop (verborgen, toont wanneer detail-paneel actief) ---
  ws_btn_terug = lv_btn_create(parent);
  lv_obj_set_size(ws_btn_terug, 140, 32);
  lv_obj_align(ws_btn_terug, LV_ALIGN_BOTTOM_MID, 0, -8);
  lv_obj_set_style_bg_color(ws_btn_terug, lv_color_hex(0x443322), 0);
  lv_obj_set_style_bg_color(ws_btn_terug, lv_color_hex(0x775533), LV_STATE_PRESSED);  // iets lichter bruin
  lv_obj_add_event_cb(ws_btn_terug, ws_terug_callback, LV_EVENT_CLICKED, NULL);
  lv_obj_add_flag(ws_btn_terug, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t* terug_lbl = lv_label_create(ws_btn_terug);
  lv_label_set_text(terug_lbl, g_lang->wifi_terug);
  lv_obj_set_style_text_font(terug_lbl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(terug_lbl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(terug_lbl);

  // --- Spinner (verborgen) ---
  ws_spinner = lv_spinner_create(parent);
  lv_obj_set_size(ws_spinner, 24, 24);
  lv_obj_align(ws_spinner, LV_ALIGN_BOTTOM_MID, 75, -14);
  lv_obj_add_flag(ws_spinner, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_arc_color(ws_spinner, lv_color_hex(0x44AAFF), LV_PART_INDICATOR);

  // --- Scrollbare netwerkenlijst ---
  ws_netwerk_lijst = lv_obj_create(parent);
  lv_obj_set_size(ws_netwerk_lijst, 152, 155);  // smaller: marge voor ronde hoeken
  lv_obj_align(ws_netwerk_lijst, LV_ALIGN_TOP_MID, 0, 62);  // verschoven mee met titel
  lv_obj_set_style_bg_color(ws_netwerk_lijst, lv_color_hex(0x0A1020), 0);
  lv_obj_set_style_border_color(ws_netwerk_lijst, lv_color_hex(0x224466), 0);
  lv_obj_set_style_border_width(ws_netwerk_lijst, 1, 0);
  lv_obj_set_style_pad_all(ws_netwerk_lijst, 3, 0);
  lv_obj_set_flex_flow(ws_netwerk_lijst, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(ws_netwerk_lijst, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  // Verticaal scrollen inschakelen zodat lange lijsten geswiped kunnen worden
  lv_obj_set_scroll_dir(ws_netwerk_lijst, LV_DIR_VER);
  lv_obj_add_flag(ws_netwerk_lijst, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(ws_netwerk_lijst, LV_SCROLLBAR_MODE_ACTIVE);
  lv_obj_set_style_width(ws_netwerk_lijst, 4, LV_PART_SCROLLBAR);
  lv_obj_set_style_bg_color(ws_netwerk_lijst, lv_color_hex(0x4488FF), LV_PART_SCROLLBAR);

  // --- Detailpaneel (verborgen, toont na klik op netwerk) ---
  ws_detail_panel = lv_obj_create(parent);
  lv_obj_set_size(ws_detail_panel, 152, 155);  // zelfde afmetingen als lijst
  lv_obj_align(ws_detail_panel, LV_ALIGN_TOP_MID, 0, 62);
  lv_obj_set_style_bg_color(ws_detail_panel, lv_color_hex(0x0A1020), 0);
  lv_obj_set_style_border_color(ws_detail_panel, lv_color_hex(0x224466), 0);
  lv_obj_set_style_border_width(ws_detail_panel, 1, 0);
  lv_obj_set_style_pad_all(ws_detail_panel, 6, 0);
  lv_obj_add_flag(ws_detail_panel, LV_OBJ_FLAG_HIDDEN);

  ws_detail_inhoud = lv_label_create(ws_detail_panel);
  lv_label_set_text(ws_detail_inhoud, "");
  lv_obj_set_style_text_font(ws_detail_inhoud, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(ws_detail_inhoud, lv_color_hex(0xCCDDFF), 0);
  lv_label_set_long_mode(ws_detail_inhoud, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(ws_detail_inhoud, lv_pct(100));

  DBG_INFO("WiFi Scanner scherm aangemaakt.");
}

// EINDE SCRIPT
