/*
  Bestand : screen_phonefinder.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Zoek Telefoon / Find Phone scherm voor ESP32C6_SmartDisplay.
            Stuurt een Telegram-bericht via de Telegram Bot API
            naar uw telefoon als u op "Zoek" drukt.

            Configuratie via het webportaal (http://[IP]/).
            Werking: Druk "Zoek" → HTTPS GET naar api.telegram.org
  Auteur  : JWP van Renen
  Versie  : v5.0.0
  Datum   : 2026-03-05 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <lvgl.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "config_manager.h"
#include "debug_log.h"
#include "lang.h"

// Verwijzing naar de globale config (gedefinieerd in .ino)
extern AppConfig g_config;

// ============================================================
// LVGL WIDGET POINTERS
// ============================================================
static lv_obj_t* pf_btn_bel        = NULL;  // Hoofdknop
static lv_obj_t* pf_label_status   = NULL;  // Status na verzenden
static lv_obj_t* pf_spinner        = NULL;  // Laad-animatie tijdens verzenden

// Vlag om dubbel-klikken te voorkomen
static volatile bool pf_bezig = false;

// ============================================================
// URL-ENCODE HULPFUNCTIE: codeert tekst voor gebruik in een URL
// ============================================================
static String pf_url_encode(const char* tekst) {
  String encoded = "";
  char hex[4];
  while (*tekst) {
    unsigned char c = (unsigned char)*tekst++;
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += (char)c;
    } else {
      snprintf(hex, sizeof(hex), "%%%02X", c);
      encoded += hex;
    }
  }
  return encoded;
}

// ============================================================
// STATUS AUTO-CLEAR TIMER: wist statusmelding na 5 seconden
// ============================================================
static void pf_status_clear_cb(lv_timer_t* timer) {
  lv_timer_del(timer);  // eenmalige timer: zelf opruimen
  if (pf_label_status != NULL) {
    lv_label_set_text(pf_label_status, "");
    lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFFFFFF), 0);
  }
}

// ============================================================
// ZOEK TELEFOON CALLBACK
// Verstuurt een Telegram-bericht wanneer de knop gedrukt wordt.
// ============================================================
static void pf_bel_callback(lv_event_t* e) {
  LV_UNUSED(e);

  if (pf_bezig) {
    DBG_WARN("Verzenden al bezig, dubbel-klik genegeerd.");
    return;
  }

  // WiFi verbinding controleren
  if (WiFi.status() != WL_CONNECTED) {
    DBG_WARN("Geen WiFi verbinding voor telefoon zoeken.");
    if (pf_label_status != NULL) {
      lv_label_set_text(pf_label_status, g_lang->pf_geen_wifi);
      lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFF4444), 0);
    }
    return;
  }

  // CallMeBot-configuratie controleren
  if (g_config.callmebot_user[0] == '\0') {
    DBG_WARN("CallMeBot niet geconfigureerd.");
    if (pf_label_status != NULL) {
      lv_label_set_text(pf_label_status, g_lang->pf_niet_ingesteld);
      lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFF8844), 0);
    }
    lv_timer_create(pf_status_clear_cb, 5000, NULL);
    return;
  }

  pf_bezig = true;

  // UI bijwerken: knop uitschakelen, spinner tonen, status bijwerken
  lv_obj_add_state(pf_btn_bel, LV_STATE_DISABLED);
  if (pf_label_status != NULL) {
    lv_label_set_text(pf_label_status, g_lang->pf_verzenden);
    lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFFFF44), 0);
  }
  if (pf_spinner != NULL) {
    lv_obj_clear_flag(pf_spinner, LV_OBJ_FLAG_HIDDEN);
  }
  lv_task_handler();  // LVGL even laten renderen zodat de UI zichtbaar bijwerkt

  // CallMeBot API URL opbouwen (GET-methode)
  // Documentatie: https://www.callmebot.com/
  String url = "https://api2.callmebot.com/start.php?source=esp32&user=";
  url += g_config.callmebot_user;
  url += "&text=";
  url += pf_url_encode(g_config.callmebot_tekst);
  url += "&lang=nl-NL-Standard-B&rpt=2&cc=no";

  DBG_INFO("CallMeBot oproep starten voor gebruiker: %s", g_config.callmebot_user);

  // HTTPS verzoek versturen
  WiFiClientSecure client;
  client.setInsecure();  // SSL certificaat niet verifiëren (acceptabel voor privé IoT)

  HTTPClient http;
  http.setTimeout(10000);  // 10 seconden timeout (Telegram kan trager zijn)

  bool verbonden = http.begin(client, url);

  int httpCode = -1;
  if (verbonden) {
    httpCode = http.GET();
    http.end();
  }

  // Status bijwerken op basis van resultaat
  if (httpCode == 200) {
    DBG_INFO("Telegram bericht verstuurd. HTTP code: %d", httpCode);
    if (pf_label_status != NULL) {
      lv_label_set_text(pf_label_status, g_lang->pf_verstuurd);
      lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0x44FF44), 0);
    }
  } else {
    DBG_WARN("Telegram mislukt. HTTP code: %d", httpCode);
    const char* fout_tekst;
    char fout_buf[48];
    if (httpCode == 401) {
      fout_tekst = g_lang->pf_fout_token;
    } else if (httpCode == 400) {
      fout_tekst = g_lang->pf_fout_chatid;
    } else {
      snprintf(fout_buf, sizeof(fout_buf), g_lang->pf_mislukt_fmt, httpCode);
      fout_tekst = fout_buf;
    }
    if (pf_label_status != NULL) {
      lv_label_set_text(pf_label_status, fout_tekst);
      lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFF4444), 0);
    }
  }

  // UI herstellen
  if (pf_spinner != NULL) {
    lv_obj_add_flag(pf_spinner, LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_clear_state(pf_btn_bel, LV_STATE_DISABLED);
  pf_bezig = false;

  // Status automatisch wissen na 5 seconden
  lv_timer_create(pf_status_clear_cb, 5000, NULL);
}

// ============================================================
// SCREEN AANMAKEN
// ============================================================
static void scherm_phonefinder_aanmaken(lv_obj_t* parent) {
  DBG_INFO("Zoek Telefoon scherm aanmaken...");

  // Achtergrond: donkergroen tint
  lv_obj_set_style_bg_color(parent, lv_color_hex(0x0A2E1A), 0);
  lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

  // --- Titellabel (y=15 voor ronde hoeken display) ---
  lv_obj_t* titel = lv_label_create(parent);
  lv_label_set_text(titel, g_lang->pf_titel);
  lv_obj_set_style_text_font(titel, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(titel, lv_color_hex(0x44FFAA), 0);
  lv_obj_align(titel, LV_ALIGN_TOP_MID, 0, 15);

  // --- Instructie tekst ---
  lv_obj_t* info_lbl = lv_label_create(parent);
  lv_label_set_text(info_lbl, g_lang->pf_instructie);
  lv_obj_set_style_text_font(info_lbl, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(info_lbl, lv_color_hex(0x88AACC), 0);
  lv_obj_set_style_text_align(info_lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(info_lbl, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(info_lbl, 148);
  lv_obj_align(info_lbl, LV_ALIGN_CENTER, 0, -15);

  // --- Status label (resultaat na verzenden) ---
  pf_label_status = lv_label_create(parent);
  lv_label_set_text(pf_label_status, "");
  lv_obj_set_style_text_font(pf_label_status, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(pf_label_status, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_align(pf_label_status, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(pf_label_status, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(pf_label_status, 148);
  lv_obj_align(pf_label_status, LV_ALIGN_CENTER, 0, 60);

  // --- Hoofdknop ---
  pf_btn_bel = lv_btn_create(parent);
  lv_obj_set_size(pf_btn_bel, 148, 36);
  lv_obj_align(pf_btn_bel, LV_ALIGN_BOTTOM_MID, 0, -12);
  lv_obj_set_style_bg_color(pf_btn_bel, lv_color_hex(0x226644), 0);
  lv_obj_set_style_bg_color(pf_btn_bel, lv_color_hex(0x3A9966), LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(pf_btn_bel, lv_color_hex(0x333333), LV_STATE_DISABLED);
  lv_obj_add_event_cb(pf_btn_bel, pf_bel_callback, LV_EVENT_CLICKED, NULL);

  lv_obj_t* btn_lbl = lv_label_create(pf_btn_bel);
  lv_label_set_text(btn_lbl, g_lang->pf_knop);
  lv_obj_set_style_text_font(btn_lbl, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(btn_lbl, lv_color_hex(0xFFFFFF), 0);
  lv_obj_center(btn_lbl);

  // --- Spinner (verborgen totdat verzenden start) ---
  pf_spinner = lv_spinner_create(parent);
  lv_obj_set_size(pf_spinner, 24, 24);
  lv_obj_align(pf_spinner, LV_ALIGN_BOTTOM_MID, 82, -18);
  lv_obj_add_flag(pf_spinner, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_arc_color(pf_spinner, lv_color_hex(0x44FFAA), LV_PART_INDICATOR);

  DBG_INFO("Zoek Telefoon scherm aangemaakt.");
}

// EINDE SCRIPT
