/*
  Bestand : config_manager.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Configuratiebeheer via NVS (Non-Volatile Storage / Preferences).
            Slaat WiFi-gegevens, Telegram-instellingen en tijdzone op
            in de interne flash van de ESP32. Geen hardcoded credentials nodig.
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-05 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <Preferences.h>
#include "debug_log.h"

// ============================================================
// CONFIGURATIE STRUCT
// Bevat alle instelbare parameters van het device.
// ============================================================
struct AppConfig {
  char wifi_ssid[64];          // WiFi netwerknaam
  char wifi_wachtwoord[64];    // WiFi wachtwoord
  char telegram_token[160];    // Telegram bot token (bijv. "123456:ABCdef...")
  char telegram_chat_id[32];   // Telegram chat ID (numeriek of @gebruikersnaam)
  char ntp_tijdzone[64];       // POSIX tijdzone (bijv. "CET-1CEST,M3.5.0,M10.5.0/3")
  char taal[4];                // Interface taal: "nl" (standaard) of "en"
};

// Standaard tijdzone (België/Nederland met zomertijd)
static const char* CONFIG_STANDAARD_TIJDZONE = "CET-1CEST,M3.5.0,M10.5.0/3";

// NVS namespace
static const char* NVS_NAMESPACE = "smartdisplay";

// ============================================================
// CONFIG LADEN UIT NVS
// Geeft true terug als een geldige configuratie gevonden is.
// ============================================================
static bool config_laden(AppConfig& cfg) {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, true);  // read-only

  bool geconfigureerd = prefs.getBool("geconfigureerd", false);

  if (geconfigureerd) {
    prefs.getString("wifi_ssid",   cfg.wifi_ssid,        sizeof(cfg.wifi_ssid));
    prefs.getString("wifi_pw",     cfg.wifi_wachtwoord,  sizeof(cfg.wifi_wachtwoord));
    prefs.getString("tg_token",    cfg.telegram_token,   sizeof(cfg.telegram_token));
    prefs.getString("tg_chatid",   cfg.telegram_chat_id, sizeof(cfg.telegram_chat_id));
    prefs.getString("tijdzone",    cfg.ntp_tijdzone,     sizeof(cfg.ntp_tijdzone));
    prefs.getString("taal",        cfg.taal,             sizeof(cfg.taal));
    // Standaard "nl" als taal niet opgeslagen is (oudere NVS)
    if (cfg.taal[0] == '\0') strncpy(cfg.taal, "nl", sizeof(cfg.taal));
    DBG_INFO("Config geladen: SSID=%s, TZ=%s, Taal=%s", cfg.wifi_ssid, cfg.ntp_tijdzone, cfg.taal);
  } else {
    // Standaardwaarden instellen
    cfg.wifi_ssid[0]       = '\0';
    cfg.wifi_wachtwoord[0] = '\0';
    cfg.telegram_token[0]  = '\0';
    cfg.telegram_chat_id[0]= '\0';
    strncpy(cfg.ntp_tijdzone, CONFIG_STANDAARD_TIJDZONE, sizeof(cfg.ntp_tijdzone) - 1);
    cfg.ntp_tijdzone[sizeof(cfg.ntp_tijdzone) - 1] = '\0';
    strncpy(cfg.taal, "nl", sizeof(cfg.taal));
    DBG_INFO("Geen config in NVS, standaardwaarden gebruikt.");
  }

  prefs.end();
  return geconfigureerd;
}

// ============================================================
// CONFIG OPSLAAN NAAR NVS
// ============================================================
static void config_opslaan(const AppConfig& cfg) {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, false);  // read-write

  prefs.putBool("geconfigureerd", true);
  prefs.putString("wifi_ssid",  cfg.wifi_ssid);
  prefs.putString("wifi_pw",    cfg.wifi_wachtwoord);
  prefs.putString("tg_token",   cfg.telegram_token);
  prefs.putString("tg_chatid",  cfg.telegram_chat_id);
  prefs.putString("tijdzone",   cfg.ntp_tijdzone);
  prefs.putString("taal",       cfg.taal);

  prefs.end();
  DBG_INFO("Config opgeslagen naar NVS.");
}

// ============================================================
// CONFIG WISSEN (factory reset)
// ============================================================
static void config_wissen() {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, false);
  prefs.clear();
  prefs.end();
  DBG_INFO("Config gewist uit NVS.");
}

// EINDE SCRIPT
