/*
  Bestand : debug_log.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Centrale debug/logging module voor ESP32C6_SmartDisplay.
            Alle Serial Monitor uitvoer loopt uitsluitend via deze macros.
            De DEBUG_SERIAL schakelaar (gedefinieerd in het hoofd-.ino bestand)
            bepaalt of uitvoer zichtbaar is in de Arduino IDE Serial Monitor.
            DEBUG_LEVEL bepaalt de hoeveelheid detail in de logging.
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <Arduino.h>

// ============================================================
// FOUTPREVENTIE: Controleer of DEBUG_SERIAL en DEBUG_LEVEL
// gedefinieerd zijn in het hoofd-.ino bestand.
// Ze mogen NIET hier gedefinieerd worden; dit bestand leest ze alleen.
// ============================================================
#ifndef DEBUG_SERIAL
  #error "DEBUG_SERIAL is niet gedefinieerd. Voeg 'const bool DEBUG_SERIAL = true/false;' toe aan het hoofd-.ino bestand."
#endif

#ifndef DEBUG_LEVEL
  #error "DEBUG_LEVEL is niet gedefinieerd. Voeg '#define DEBUG_LEVEL 3' toe aan het hoofd-.ino bestand."
#endif

// ============================================================
// DEBUG LEVEL DEFINITIES
// 0 = uit (geen output, ook al is DEBUG_SERIAL true)
// 1 = alleen errors
// 2 = errors + warnings
// 3 = errors + warnings + info (standaard voor ontwikkeling)
// 4 = verbose (alles, inclusief touch events, LVGL updates)
// ============================================================
#define DBG_LEVEL_UIT       0
#define DBG_LEVEL_ERROR     1
#define DBG_LEVEL_WARNING   2
#define DBG_LEVEL_INFO      3
#define DBG_LEVEL_VERBOSE   4

// ============================================================
// LOGGING MACROS
// Gebruik: DBG_INFO("Tekst: %d", waarde);
//
// Werking: als DEBUG_SERIAL false is, wordt NIETS uitgeschreven,
// ongeacht het ingestelde DEBUG_LEVEL.
//
// Printf-stijl formatting wordt ondersteund via Serial.printf().
// ============================================================

// ERROR: kritieke fouten die de werking belemmeren
// Voorbeeld: "I2C initialisatie mislukt"
#define DBG_ERROR(fmt, ...) \
  do { \
    if (DEBUG_SERIAL && DEBUG_LEVEL >= DBG_LEVEL_ERROR) { \
      Serial.printf("[FOUT][%lu] " fmt "\n", millis(), ##__VA_ARGS__); \
    } \
  } while(0)

// WARNING: niet-kritieke afwijkingen die aandacht verdienen
// Voorbeeld: "WiFi signaal zwak, RSSI < -80 dBm"
#define DBG_WARN(fmt, ...) \
  do { \
    if (DEBUG_SERIAL && DEBUG_LEVEL >= DBG_LEVEL_WARNING) { \
      Serial.printf("[WAARSCH][%lu] " fmt "\n", millis(), ##__VA_ARGS__); \
    } \
  } while(0)

// INFO: normale operationele meldingen
// Voorbeeld: "NTP tijd gesynchroniseerd", "WiFi verbonden"
#define DBG_INFO(fmt, ...) \
  do { \
    if (DEBUG_SERIAL && DEBUG_LEVEL >= DBG_LEVEL_INFO) { \
      Serial.printf("[INFO][%lu] " fmt "\n", millis(), ##__VA_ARGS__); \
    } \
  } while(0)

// VERBOSE: gedetailleerde debug-uitvoer voor diepe probleemoplossing
// Voorbeeld: touch coördinaten, individuele LVGL events
#define DBG_VERBOSE(fmt, ...) \
  do { \
    if (DEBUG_SERIAL && DEBUG_LEVEL >= DBG_LEVEL_VERBOSE) { \
      Serial.printf("[UITVR][%lu] " fmt "\n", millis(), ##__VA_ARGS__); \
    } \
  } while(0)

// ============================================================
// BOOT INFORMATIE HELPER
// Toont versie, reset-reden en heap-status bij opstarten.
// Roep aan in setup() NADAT Serial.begin() is uitgevoerd.
// ============================================================
inline void dbg_boot_info(const char* versie) {
  if (!DEBUG_SERIAL) return;

  Serial.println("===========================================");
  Serial.print  ("[BOOT] Versie    : ");
  Serial.println(versie);
  Serial.print  ("[BOOT] Datum     : ");
  Serial.println(__DATE__ " " __TIME__);

  // Reset-reden loggen: helpt bij diagnoseren van onverwachte reboots
  // (watchdog, power-on, software reset, etc.)
  esp_reset_reason_t reden = esp_reset_reason();
  Serial.print  ("[BOOT] Resetreden: ");
  switch (reden) {
    case ESP_RST_POWERON:  Serial.println("Inschakelen (Power-on)"); break;
    case ESP_RST_EXT:      Serial.println("Externe reset pin");      break;
    case ESP_RST_SW:       Serial.println("Software reset");          break;
    case ESP_RST_PANIC:    Serial.println("PANIC / Exception!");      break;
    case ESP_RST_INT_WDT:  Serial.println("Interrupt Watchdog!");     break;
    case ESP_RST_TASK_WDT: Serial.println("Task Watchdog!");          break;
    case ESP_RST_WDT:      Serial.println("Andere Watchdog!");        break;
    case ESP_RST_DEEPSLEEP:Serial.println("Wakker uit deep sleep");   break;
    case ESP_RST_BROWNOUT: Serial.println("Brownout (te lage spanning)!"); break;
    default:               Serial.println("Onbekend");                break;
  }

  // Vrij heap-geheugen: essentieel bij LVGL + WiFi + BLE combinatie
  Serial.printf("[BOOT] Vrij heap : %u bytes\n", esp_get_free_heap_size());
  Serial.printf("[BOOT] Min heap  : %u bytes\n", esp_get_minimum_free_heap_size());

  Serial.println("===========================================");
  Serial.flush();
}

// EINDE SCRIPT
