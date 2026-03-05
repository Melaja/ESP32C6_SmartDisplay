/*
  Bestand : screen_datetime.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Datum/Tijd scherm voor ESP32C6_SmartDisplay.
            Toont de huidige datum in het Nederlands (bijv. "07 mei 1955 zaterdag")
            en de tijd in 24-uurs formaat (bijv. "23:59:59").
            De tijd wordt gesynchroniseerd via NTP over WiFi.
            Bijwerken gebeurt via een LVGL timer elke seconde.
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <lvgl.h>
#include <time.h>
#include <WiFi.h>
#include "debug_log.h"
#include "lang.h"

// ============================================================
// LVGL WIDGET POINTERS (statisch: leven mee met het tabblad)
// ============================================================
static lv_obj_t* dt_label_tijd      = NULL;  // Grote tijdweergave "23:59:59"
static lv_obj_t* dt_label_datum     = NULL;  // Datumweergave "07 mei 2026 woensdag"
static lv_obj_t* dt_label_status    = NULL;  // NTP sync status
static lv_obj_t* dt_label_ip        = NULL;  // IP-adres voor herconfiguratie
static lv_timer_t* dt_timer         = NULL;  // LVGL timer voor secundelijkse update

// NTP synchronisatie status bijhouden
static bool dt_ntp_gesynchroniseerd = false;

// ============================================================
// VOORWAARTSE DECLARATIES (intern gebruik)
// ============================================================
static void dt_timer_callback(lv_timer_t* timer);
static void dt_update_weergave(void);

// ============================================================
// SCREEN AANMAKEN
// Roep aan na LVGL initialisatie om het DateTime-tabblad te vullen.
// parent: het lv_obj_t* van het tabblad (van lv_tabview_add_tab)
// ============================================================
static void scherm_datetime_aanmaken(lv_obj_t* parent) {
  DBG_INFO("DateTime scherm aanmaken...");

  // Achtergrond van het tabblad: donkerblauw/marineblauw
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x0A0A2E), 0);
  lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

  // --- Decoratieve horizontale lijn boven de tijd ---
  lv_obj_t* lijn_boven = lv_obj_create(parent);
  lv_obj_set_size(lijn_boven, 140, 2);
  lv_obj_align(lijn_boven, LV_ALIGN_CENTER, 0, -70);
  lv_obj_set_style_bg_color(lijn_boven, lv_color_hex(0x4488FF), 0);
  lv_obj_set_style_border_width(lijn_boven, 0, 0);
  lv_obj_set_style_radius(lijn_boven, 1, 0);

  // --- Tijdlabel (groot, 32pt) ---
  // Centerpunt van het scherm, iets boven het midden
  dt_label_tijd = lv_label_create(parent);
  lv_label_set_text(dt_label_tijd, "--:--:--");
  lv_obj_set_style_text_font(dt_label_tijd, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(dt_label_tijd, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(dt_label_tijd, LV_ALIGN_CENTER, 0, -45);

  // --- Decoratieve horizontale lijn onder de tijd ---
  lv_obj_t* lijn_onder = lv_obj_create(parent);
  lv_obj_set_size(lijn_onder, 140, 2);
  lv_obj_align(lijn_onder, LV_ALIGN_CENTER, 0, -15);
  lv_obj_set_style_bg_color(lijn_onder, lv_color_hex(0x4488FF), 0);
  lv_obj_set_style_border_width(lijn_onder, 0, 0);
  lv_obj_set_style_radius(lijn_onder, 1, 0);

  // --- Datumlabel (normaal, 16pt) ---
  // Toont: "04 maart 2026 woensdag"
  dt_label_datum = lv_label_create(parent);
  lv_label_set_text(dt_label_datum, "--");
  lv_obj_set_style_text_font(dt_label_datum, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(dt_label_datum, lv_color_hex(0xCCDDFF), 0);
  lv_obj_align(dt_label_datum, LV_ALIGN_CENTER, 0, 10);
  lv_label_set_long_mode(dt_label_datum, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(dt_label_datum, 160);
  lv_obj_set_style_text_align(dt_label_datum, LV_TEXT_ALIGN_CENTER, 0);

  // --- NTP status label (klein, 12pt) ---
  // Toont: "NTP: gesynchroniseerd" of "NTP: wachten..."
  dt_label_status = lv_label_create(parent);
  lv_label_set_text(dt_label_status, g_lang->ntp_wachten);
  lv_obj_set_style_text_font(dt_label_status, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(dt_label_status, lv_color_hex(0x888899), 0);
  lv_obj_align(dt_label_status, LV_ALIGN_CENTER, 0, 50);

  // --- IP-adres label (klein, 12pt) ---
  // Toont het IP-adres zodat de gebruiker via browser kan herconfigureren
  dt_label_ip = lv_label_create(parent);
  lv_label_set_text(dt_label_ip, "");
  lv_obj_set_style_text_font(dt_label_ip, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(dt_label_ip, lv_color_hex(0x4488FF), 0);
  lv_obj_set_style_text_align(dt_label_ip, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(dt_label_ip, LV_ALIGN_CENTER, 0, 68);

  // --- LVGL timer voor 1-seconde updates ---
  // 1000ms interval, herhaalt oneindig (repeat_count = -1)
  // De callback werkt de tijd en datum labels bij
  dt_timer = lv_timer_create(dt_timer_callback, 1000, NULL);

  // Eerste weergave direct bijwerken (niet wachten op eerste timer tick)
  dt_update_weergave();

  DBG_INFO("DateTime scherm aangemaakt.");
}

// ============================================================
// INTERNE FUNCTIES
// ============================================================

// Werk de tijds- en datumlabels bij met de huidige systeemtijd.
// Systeemtijd wordt via NTP gesynchroniseerd (zie wifi_verbinden() in .ino).
static void dt_update_weergave(void) {
  time_t nu = time(NULL);

  // Controleer of de tijd al gesynchroniseerd is:
  // Een waarde van 0 of vlak na epoch (jaar 1970) betekent GEEN NTP sync
  if (nu < 1000000000L) {
    // Nog niet gesynchroniseerd: toon wachtbericht
    if (dt_label_tijd != NULL) {
      lv_label_set_text(dt_label_tijd, "--:--:--");
    }
    if (dt_label_datum != NULL) {
      lv_label_set_text(dt_label_datum, g_lang->synchroniseren);
    }
    DBG_VERBOSE("Tijd nog niet gesynchroniseerd (epoch: %ld)", (long)nu);
    return;
  }

  // Tijd is beschikbaar: converteer naar lokale tijd (tijdzone via NTP_TIJDZONE)
  struct tm tijdinfo;
  localtime_r(&nu, &tijdinfo);

  // --- Tijdstring opbouwen: "23:59:59" ---
  char tijdbuf[12];
  snprintf(tijdbuf, sizeof(tijdbuf), "%02d:%02d:%02d",
           tijdinfo.tm_hour, tijdinfo.tm_min, tijdinfo.tm_sec);

  // --- Datumstring opbouwen: "04 maart 2026 woensdag" ---
  // tm_mon is 0-gebaseerd, tm_wday 0 = zondag
  char datumbuf[48];
  snprintf(datumbuf, sizeof(datumbuf), "%02d %s %04d\n%s",
           tijdinfo.tm_mday,
           g_lang->maanden[tijdinfo.tm_mon],
           tijdinfo.tm_year + 1900,
           g_lang->dagen[tijdinfo.tm_wday]);

  // Labels bijwerken
  if (dt_label_tijd != NULL) {
    lv_label_set_text(dt_label_tijd, tijdbuf);
  }
  if (dt_label_datum != NULL) {
    lv_label_set_text(dt_label_datum, datumbuf);
  }

  // NTP status bijwerken (eerste keer gesynchroniseerd)
  if (!dt_ntp_gesynchroniseerd && dt_label_status != NULL) {
    dt_ntp_gesynchroniseerd = true;
    lv_label_set_text(dt_label_status, g_lang->ntp_gesynchroniseerd);
    lv_obj_set_style_text_color(dt_label_status, lv_color_hex(0x44BB44), 0);
    DBG_INFO("NTP tijdsynchronisatie geslaagd: %s", tijdbuf);
  }

  // IP-adres bijwerken
  if (dt_label_ip != NULL) {
    if (WiFi.status() == WL_CONNECTED) {
      String ip = "http://" + WiFi.localIP().toString() + "/";
      lv_label_set_text(dt_label_ip, ip.c_str());
    } else {
      lv_label_set_text(dt_label_ip, "");
    }
  }

  DBG_VERBOSE("Tijd bijgewerkt: %s | %s", tijdbuf, datumbuf);
}

// LVGL timer callback: wordt elke seconde aangeroepen door LVGL
static void dt_timer_callback(lv_timer_t* timer) {
  // timer parameter niet gebruikt (vereist door LVGL callback-signatuur)
  LV_UNUSED(timer);
  dt_update_weergave();
}

// Vernietig de LVGL timer wanneer het scherm niet meer actief is.
// Aanroepen wanneer van tabblad gewisseld wordt (optioneel, bespaart CPU).
static void scherm_datetime_opruimen(void) {
  if (dt_timer != NULL) {
    lv_timer_del(dt_timer);
    dt_timer = NULL;
    DBG_VERBOSE("DateTime timer verwijderd.");
  }
}

// EINDE SCRIPT
