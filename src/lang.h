/*
  Bestand : lang.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Meertalige interface (Nederlands / English).
            Bevat alle zichtbare UI-teksten in beide talen.
            Stel g_lang in op &LANG_NL of &LANG_EN na config laden.
  Auteur  : JWP van Renen
  Versie  : v1.1.0
  Datum   : 2026-03-05 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <lvgl.h>

// ============================================================
// MAAND- EN DAGNAAM ARRAYS PER TAAL
// ============================================================

// Nederlands
static const char* const DT_MAANDEN_NL[] = {
  "januari", "februari", "maart",     "april",   "mei",      "juni",
  "juli",    "augustus", "september", "oktober",  "november", "december"
};
static const char* const DT_DAGEN_NL[] = {
  "zondag", "maandag", "dinsdag", "woensdag",
  "donderdag", "vrijdag", "zaterdag"
};

// English
static const char* const DT_MAANDEN_EN[] = {
  "January", "February", "March",    "April",   "May",      "June",
  "July",    "August",   "September","October",  "November", "December"
};
static const char* const DT_DAGEN_EN[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday",
  "Thursday", "Friday", "Saturday"
};

// ============================================================
// TAAL STRUCT: alle UI-teksten per taal
// ============================================================
struct Lang {
  // --- Datum/Tijd scherm ---
  const char* ntp_wachten;           // "NTP: wachten..." / "NTP: waiting..."
  const char* ntp_gesynchroniseerd;  // "NTP: gesynchroniseerd" / "NTP: synchronized"
  const char* synchroniseren;        // "Synchroniseren..." / "Synchronizing..."
  const char* const* maanden;        // Array van 12 maandnamen
  const char* const* dagen;          // Array van 7 dagnamen

  // --- WiFi Scanner scherm ---
  const char* wifi_druk_scan;        // "Druk op Scan" / "Press Scan"
  const char* wifi_scannen;          // "Scannen..." / "Scanning..."
  const char* wifi_scan_mislukt;     // "Scan mislukt" / "Scan failed"
  const char* wifi_netw_enkv;        // "netwerk gevonden" / "network found"
  const char* wifi_netw_mv;          // "netwerken gevonden" / "networks found"
  const char* wifi_verborgen;        // "(verborgen netwerk)" / "(hidden network)"
  const char* wifi_geen_netwerken;   // "Geen netwerken gevonden." / "No networks found."
  const char* wifi_terug;            // Terug-knop label
  const char* wifi_detail_fmt;       // snprintf format: %s SSID, %s BSSID,
                                     //   %ld rssi, %s sterkte, %ld kanaal, %ld freq, %s bev.
  // Signaalsterkte
  const char* sig_uitstekend;
  const char* sig_goed;
  const char* sig_redelijk;
  const char* sig_zwak;
  const char* sig_zeer_zwak;
  // Beveiligingstype
  const char* sec_open;
  const char* sec_wep;
  const char* sec_onbekend;

  // --- Zoek Telefoon / Find Phone scherm ---
  const char* pf_titel;              // Schermtitel
  const char* pf_instructie;         // Instructietekst
  const char* pf_knop;               // Knooptekst
  const char* pf_geen_wifi;          // Geen WiFi foutmelding
  const char* pf_niet_ingesteld;     // Telegram niet geconfigureerd
  const char* pf_verzenden;          // Bezig met verzenden
  const char* pf_verstuurd;          // Succes
  const char* pf_fout_token;         // HTTP 401
  const char* pf_fout_chatid;        // HTTP 400
  const char* pf_mislukt_fmt;        // Andere fout, %d = HTTP code

  // --- Web configuratie AP-modus scherm ---
  const char* wc_ap_titel;           // "Instelmodus" / "Setup mode"
  const char* wc_ap_instructie;      // "Verbind met WiFi:..." / "Connect to WiFi:..."
};

// ============================================================
// NEDERLANDS
// ============================================================
static const Lang LANG_NL = {
  // Datum/Tijd
  "NTP: wachten...",
  "NTP: gesynchroniseerd",
  "Synchroniseren...",
  DT_MAANDEN_NL,
  DT_DAGEN_NL,
  // WiFi Scanner
  "Druk op Scan",
  "Scannen...",
  "Scan mislukt",
  "netwerk gevonden",
  "netwerken gevonden",
  "(verborgen netwerk)",
  "Geen netwerken gevonden.",
  LV_SYMBOL_LEFT " Terug naar lijst",
  "SSID:\n  %s\n\nBSSID:\n  %s\n\nSignaal:\n  %ld dBm (%s)\n\nKanaal:\n  %ld (%ld MHz)\n\nBeveiliging:\n  %s",
  "Uitstekend", "Goed", "Redelijk", "Zwak", "Zeer zwak",
  "Open (geen beveiliging)", "WEP (verouderd, onveilig)", "Onbekend",
  // Zoek Telefoon
  LV_SYMBOL_CALL " Zoek Telefoon",
  "Druk de knop om uw\ntelefoon op te bellen\nvia CallMeBot.",
  LV_SYMBOL_CALL " Bellen",
  LV_SYMBOL_CLOSE " Geen WiFi verbinding",
  LV_SYMBOL_CLOSE " CallMeBot niet ingesteld",
  "Telefoon bellen...",
  LV_SYMBOL_OK " Oproep gestart!",
  LV_SYMBOL_CLOSE " Fout (401)",
  LV_SYMBOL_CLOSE " Fout (400)",
  LV_SYMBOL_CLOSE " Mislukt (%d)",
  // Web configuratie AP-modus
  LV_SYMBOL_SETTINGS " Instelmodus",
  "Verbind met WiFi:\nESP32-Config\n\nOpen browser:\n192.168.4.1"
};

// ============================================================
// ENGLISH
// ============================================================
static const Lang LANG_EN = {
  // Date/Time
  "NTP: waiting...",
  "NTP: synchronized",
  "Synchronizing...",
  DT_MAANDEN_EN,
  DT_DAGEN_EN,
  // WiFi Scanner
  "Press Scan",
  "Scanning...",
  "Scan failed",
  "network found",
  "networks found",
  "(hidden network)",
  "No networks found.",
  LV_SYMBOL_LEFT " Back to list",
  "SSID:\n  %s\n\nBSSID:\n  %s\n\nSignal:\n  %ld dBm (%s)\n\nChannel:\n  %ld (%ld MHz)\n\nSecurity:\n  %s",
  "Excellent", "Good", "Fair", "Weak", "Very weak",
  "Open (no security)", "WEP (outdated, insecure)", "Unknown",
  // Find Phone
  LV_SYMBOL_CALL " Find Phone",
  "Press the button to\ncall your phone\nvia CallMeBot.",
  LV_SYMBOL_CALL " Call",
  LV_SYMBOL_CLOSE " No WiFi connection",
  LV_SYMBOL_CLOSE " CallMeBot not configured",
  "Calling phone...",
  LV_SYMBOL_OK " Call started!",
  LV_SYMBOL_CLOSE " Error (401)",
  LV_SYMBOL_CLOSE " Error (400)",
  LV_SYMBOL_CLOSE " Failed (%d)",
  // Web config AP mode screen
  LV_SYMBOL_SETTINGS " Setup mode",
  "Connect to WiFi:\nESP32-Config\n\nOpen browser:\n192.168.4.1"
};

// ============================================================
// ACTIEVE TAAL POINTER
// Gedefinieerd in ESP32C6_SmartDisplay.ino.
// Ingesteld op basis van g_config.taal na config laden.
// ============================================================
extern const Lang* g_lang;

// EINDE SCRIPT
