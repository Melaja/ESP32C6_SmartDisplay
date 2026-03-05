/*
  Bestand : wifi_config_example.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Voorbeeldbestand met placeholders voor WiFi-inloggegevens.
            KOPIEER dit bestand naar "wifi_config.h" en vul je eigen
            gegevens in. wifi_config.h staat in .gitignore en mag
            NOOIT worden gecommit naar versiebeheersystemen.
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

// ============================================================
// WiFi NETWERKINSTELLINGEN
// Vul je eigen SSID en wachtwoord in wifi_config.h
// ============================================================

// Naam van je WiFi-netwerk (SSID)
#define WIFI_SSID     "JOUW_WIFI_NAAM_HIER"

// Wachtwoord van je WiFi-netwerk
#define WIFI_WACHTWOORD  "JOUW_WIFI_WACHTWOORD_HIER"

// ============================================================
// NTP TIJDSERVER INSTELLINGEN
// Tijdzone voor België/Nederland: CET-1CEST,M3.5.0,M10.5.0/3
//   CET  = Central European Time = UTC+1 (winter)
//   CEST = Central European Summer Time = UTC+2 (zomer)
//   M3.5.0 = overgaan op zomertijd: laatste zondag in maart
//   M10.5.0/3 = terugkeren naar wintertijd: laatste zondag in oktober
// ============================================================
#define NTP_SERVER_1  "pool.ntp.org"
#define NTP_SERVER_2  "time.nist.gov"
#define NTP_TIJDZONE  "CET-1CEST,M3.5.0,M10.5.0/3"

// WiFi verbindingstimeout in milliseconden (30 seconden is redelijk)
#define WIFI_TIMEOUT_MS 30000

// EINDE SCRIPT
