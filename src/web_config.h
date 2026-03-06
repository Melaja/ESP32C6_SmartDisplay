/*
  Bestand : web_config.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Web-gebaseerd configuratieportaal voor ESP32C6_SmartDisplay.
            - AP-modus: start als "ESP32-Config" accesspoint
            - Captive portal: alle DNS-verzoeken → 192.168.4.1
            - WebServer op port 80: HTML-formulier voor configuratie
            - STA-modus: ook bereikbaar via http://[IP]/ na WiFi-verbinding
            - Na opslaan: herstart met nieuwe instellingen
            - Volledig tweetalig: Nederlands / English (op basis van cfg.taal)
  Auteur  : JWP van Renen
  Versie  : v2.4.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <lvgl.h>
#include "config_manager.h"
#include "debug_log.h"
#include "lang.h"

// ============================================================
// CONSTANTEN
// ============================================================
static const char* AP_SSID     = "ESP32-Config";
static const char* AP_IP_STR   = "192.168.4.1";
static const uint8_t DNS_PORT  = 53;

// ============================================================
// GLOBALE OBJECTEN (webserver + DNS)
// ============================================================
static WebServer wc_server(80);
static DNSServer wc_dns;

// Verwijzing naar de huidige config (ingesteld door wc_init)
static AppConfig* wc_cfg_ptr = nullptr;

// ============================================================
// HTML HULPFUNCTIE: URL-decode een string uit POST-formulier
// ============================================================
static String wc_url_decode(const String& src) {
  String decoded = "";
  char temp[3] = {0};
  for (int i = 0; i < (int)src.length(); i++) {
    if (src[i] == '+') {
      decoded += ' ';
    } else if (src[i] == '%' && i + 2 < (int)src.length()) {
      temp[0] = src[i + 1];
      temp[1] = src[i + 2];
      decoded += (char)strtol(temp, nullptr, 16);
      i += 2;
    } else {
      decoded += src[i];
    }
  }
  return decoded;
}

// ============================================================
// HTML PAGINA GENEREREN
// Vult huidige config-waarden in als standaard.
// Tweetalig op basis van cfg.taal ("nl" of "en").
// ============================================================
static String wc_html_formulier(const AppConfig& cfg, const String& melding = "") {
  bool isEn = (strcmp(cfg.taal, "en") == 0);

  String html = F(
    "<!DOCTYPE html><html><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
  );
  html += "<title>";
  html += isEn ? "ESP32 Configuration" : "ESP32 Configuratie";
  html += F(
    "</title>"
    "<style>"
    "body{font-family:Arial,sans-serif;background:#0d0d1f;color:#dde;padding:16px;max-width:520px;margin:auto}"
    "h1{color:#44AAFF;margin-bottom:4px}h2{color:#88CCFF;font-size:1em;margin:18px 0 6px;border-bottom:1px solid #334;padding-bottom:4px}"
    "label{display:block;font-size:.85em;color:#aab;margin-bottom:2px}"
    "input{width:100%;padding:8px;margin-bottom:12px;box-sizing:border-box;background:#0a0a1e;color:#eef;border:1px solid #336;border-radius:4px;font-size:.95em}"
    "input:focus{outline:none;border-color:#44AAFF}"
    ".tip{font-size:.78em;color:#778;margin:-8px 0 10px;line-height:1.4}"
    "button{width:100%;padding:11px;border:none;border-radius:4px;cursor:pointer;font-size:1em;margin-top:4px}"
    ".btn-save{background:#1a4466;color:#fff}.btn-save:hover{background:#2e6699}"
    ".btn-reset{background:#442211;color:#dca;margin-top:10px}.btn-reset:hover{background:#663322}"
    ".melding{background:#1a3a1a;border:1px solid #4a8a4a;color:#8f8;padding:10px;border-radius:4px;margin-bottom:14px}"
    ".fout{background:#3a1a1a;border:1px solid #8a4a4a;color:#f88}"
    "</style></head><body>"
  );
  html += "<h1>&#9881; ";
  html += isEn ? "ESP32 Configuration" : "ESP32 Configuratie";
  html += F("</h1>");
  html += F("<p style='color:#778;font-size:.85em'>Waveshare ESP32-C6 SmartDisplay</p>");

  if (melding.length() > 0) {
    html += "<div class='melding'>" + melding + "</div>";
  }

  html += F("<form action='/opslaan' method='post'>");

  // --- WiFi sectie ---
  html += F("<h2>&#128246; WiFi</h2>");
  html += "<label>";
  html += isEn ? "Network name (SSID)" : "Netwerknaam (SSID)";
  html += "</label>";
  html += "<input type='text' name='wifi_ssid' value='" + String(cfg.wifi_ssid) + "' required placeholder='";
  html += isEn ? "Your WiFi network name" : "Naam van uw WiFi netwerk";
  html += "'>";
  html += "<label>";
  html += isEn ? "Password" : "Wachtwoord";
  html += "</label>";
  html += "<input type='password' name='wifi_pw' value='" + String(cfg.wifi_wachtwoord) + "' placeholder='";
  html += isEn ? "WiFi password" : "WiFi wachtwoord";
  html += "'>";

  // --- CallMeBot sectie ---
  html += "<h2>&#128222; CallMeBot (";
  html += isEn ? "Find Phone" : "Zoek Telefoon";
  html += ")</h2>";
  if (isEn) {
    html += F(
      "<p class='tip'><b>Step 1:</b> Open Telegram &rarr; search <b>@CallMeBot_txtbot</b> &rarr; send <code>/start</code>.<br>"
      "<b>Step 2:</b> Follow the instructions to link your phone number.<br>"
      "<b>Step 3:</b> Your Telegram username (e.g. <code>@YourName</code>) is your CallMeBot username.</p>"
    );
  } else {
    html += F(
      "<p class='tip'><b>Stap 1:</b> Open Telegram &rarr; zoek <b>@CallMeBot_txtbot</b> &rarr; stuur <code>/start</code>.<br>"
      "<b>Stap 2:</b> Volg de instructies om uw telefoonnummer te koppelen.<br>"
      "<b>Stap 3:</b> Uw Telegram-gebruikersnaam (bijv. <code>@UwNaam</code>) is uw CallMeBot-gebruikersnaam.</p>"
    );
  }
  // CallMeBot gebruikersnaam: als al ingesteld password-veld, anders tekstveld.
  html += "<label>";
  html += isEn ? "CallMeBot username" : "CallMeBot gebruikersnaam";
  html += "</label>";
  if (cfg.callmebot_user[0] != '\0') {
    html += "<input type='password' name='cb_user' value='' placeholder='";
    html += isEn ? "Saved - leave empty to keep current value" : "Opgeslagen - laat leeg om te behouden";
    html += "'>";
  } else {
    html += F("<input type='text' name='cb_user' value='' placeholder='@UwNaam'>");
  }
  // Beltekst: gewone tekst, geen gevoelige data, huidige waarde tonen.
  html += "<label>";
  html += isEn ? "Call text (spoken message)" : "Beltekst (gesproken bericht)";
  html += "</label>";
  html += "<input type='text' name='cb_tekst' value='" + String(cfg.callmebot_tekst) + "' placeholder='";
  html += isEn ? "Find my phone" : "Zoek mijn telefoon";
  html += "'>";

  // --- Tijdzone sectie ---
  html += "<h2>&#128336; ";
  html += isEn ? "Timezone" : "Tijdzone";
  html += "</h2>";
  html += F("<p class='tip'>");
  html += isEn
    ? "Use POSIX notation. Belgium/Netherlands: <code>CET-1CEST,M3.5.0,M10.5.0/3</code><br>"
    : "Gebruik POSIX-notatie. Belgi&euml;/Nederland: <code>CET-1CEST,M3.5.0,M10.5.0/3</code><br>";
  html += isEn ? "&#128279; All timezones: " : "&#128279; Alle tijdzones: ";
  html += F(
    "<a href='https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv' "
    "target='_blank' style='color:#44AAFF'>"
    "github.com/nayarsystems/posix_tz_db</a>"
    "</p>"
  );
  html += "<label>";
  html += isEn ? "Timezone (POSIX)" : "Tijdzone (POSIX)";
  html += "</label>";
  html += "<input type='text' name='tijdzone' value='" + String(cfg.ntp_tijdzone) + "' placeholder='CET-1CEST,M3.5.0,M10.5.0/3'>";

  // --- Taal / Language sectie ---
  html += F("<h2>&#127757; Taal / Language</h2>");
  html += F("<div style='display:flex;gap:24px;margin-bottom:14px'>");
  html += "<label style='display:flex;align-items:center;gap:6px;cursor:pointer'>"
          "<input type='radio' name='taal' value='nl'" + String(isEn ? "" : " checked") + "> Nederlands</label>";
  html += "<label style='display:flex;align-items:center;gap:6px;cursor:pointer'>"
          "<input type='radio' name='taal' value='en'" + String(isEn ? " checked" : "") + "> English</label>";
  html += F("</div>");

  // --- Knoppen ---
  html += "<button type='submit' class='btn-save'>&#128190; ";
  html += isEn ? "Save &amp; Restart" : "Opslaan &amp; Herstarten";
  html += F("</button>");
  html += F("</form>");

  html += "<form action='/wissen' method='post' onsubmit=\"return confirm('";
  html += isEn ? "Clear all settings and restart?" : "Alle instellingen wissen en herstarten?";
  html += "')\">";
  html += "<button type='submit' class='btn-reset'>&#128465; ";
  html += isEn ? "Clear configuration (factory reset)" : "Wis configuratie (factory reset)";
  html += F("</button></form>");

  html += F("</body></html>");
  return html;
}

// ============================================================
// ROUTE: GET / → toon formulier
// ============================================================
static void wc_handle_root() {
  DBG_VERBOSE("WebConfig: GET /");
  wc_server.send(200, "text/html; charset=utf-8",
    wc_html_formulier(wc_cfg_ptr ? *wc_cfg_ptr : AppConfig{}));
}

// ============================================================
// ROUTE: POST /opslaan → config opslaan + herstarten
// ============================================================
static void wc_handle_opslaan() {
  DBG_INFO("WebConfig: POST /opslaan");

  if (!wc_cfg_ptr) {
    wc_server.send(500, "text/plain", "Config pointer niet ingesteld");
    return;
  }

  AppConfig& cfg = *wc_cfg_ptr;

  // Lees POST-velden (URL-decoded)
  String ssid    = wc_url_decode(wc_server.arg("wifi_ssid"));
  String pw      = wc_url_decode(wc_server.arg("wifi_pw"));
  String cbuser  = wc_url_decode(wc_server.arg("cb_user"));
  String cbtekst = wc_url_decode(wc_server.arg("cb_tekst"));
  String tijdzone= wc_url_decode(wc_server.arg("tijdzone"));
  ssid.trim(); cbuser.trim(); cbtekst.trim();
  String taal    = wc_server.arg("taal");
  if (taal != "nl" && taal != "en") taal = "nl";
  bool isEn = (taal == "en");

  // Validatie: SSID is verplicht
  if (ssid.length() == 0) {
    String fout = "<span class='fout'>&#10060; ";
    fout += isEn ? "Error: WiFi SSID cannot be empty." : "Fout: WiFi SSID mag niet leeg zijn.";
    fout += "</span>";
    wc_server.send(200, "text/html; charset=utf-8",
      wc_html_formulier(cfg, fout));
    return;
  }
  if (tijdzone.length() == 0) {
    tijdzone = CONFIG_STANDAARD_TIJDZONE;
  }

  // Velden kopiëren naar config (null-terminatie inbegrepen via - 1 + expliciete nul)
  strncpy(cfg.wifi_ssid,       ssid.c_str(),    sizeof(cfg.wifi_ssid) - 1);
  cfg.wifi_ssid[sizeof(cfg.wifi_ssid) - 1] = '\0';

  strncpy(cfg.wifi_wachtwoord, pw.c_str(),      sizeof(cfg.wifi_wachtwoord) - 1);
  cfg.wifi_wachtwoord[sizeof(cfg.wifi_wachtwoord) - 1] = '\0';

  // CallMeBot gebruikersnaam: alleen overschrijven als de gebruiker iets heeft ingevuld.
  // Leeg veld = "bestaande waarde behouden" (veld werd als password-placeholder getoond).
  if (cbuser.length() > 0) {
    strncpy(cfg.callmebot_user, cbuser.c_str(), sizeof(cfg.callmebot_user) - 1);
    cfg.callmebot_user[sizeof(cfg.callmebot_user) - 1] = '\0';
    DBG_INFO("CallMeBot gebruiker bijgewerkt.");
  } else {
    DBG_INFO("CallMeBot gebruiker ongewijzigd (leeg ingestuurd).");
  }
  if (cbtekst.length() > 0) {
    strncpy(cfg.callmebot_tekst, cbtekst.c_str(), sizeof(cfg.callmebot_tekst) - 1);
    cfg.callmebot_tekst[sizeof(cfg.callmebot_tekst) - 1] = '\0';
    DBG_INFO("CallMeBot tekst bijgewerkt.");
  }

  strncpy(cfg.ntp_tijdzone, tijdzone.c_str(), sizeof(cfg.ntp_tijdzone) - 1);
  cfg.ntp_tijdzone[sizeof(cfg.ntp_tijdzone) - 1] = '\0';

  strncpy(cfg.taal, taal.c_str(), sizeof(cfg.taal) - 1);
  cfg.taal[sizeof(cfg.taal) - 1] = '\0';

  // Opslaan naar NVS
  config_opslaan(cfg);
  DBG_INFO("Config opgeslagen. Herstarten in 2s...");

  // Bevestigingspagina
  String bevestiging = F(
    "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
  );
  bevestiging += "<title>";
  bevestiging += isEn ? "Saved" : "Opgeslagen";
  bevestiging += F(
    "</title>"
    "<style>body{font-family:Arial;background:#0d0d1f;color:#dde;text-align:center;padding:40px}"
    "h1{color:#44FF88}.info{color:#88CCFF;margin-top:10px}</style></head>"
    "<body><h1>&#10003; "
  );
  bevestiging += isEn ? "Configuration saved!" : "Configuratie opgeslagen!";
  bevestiging += F("</h1><p class='info'>");
  bevestiging += isEn
    ? "The device is restarting and connecting to your WiFi..."
    : "Het device herstart nu en verbindt met uw WiFi...";
  bevestiging += F("</p><p style='color:#778;font-size:.85em'>");
  bevestiging += isEn ? "You can close this window." : "U kunt dit venster sluiten.";
  bevestiging += F("</p></body></html>");

  wc_server.send(200, "text/html; charset=utf-8", bevestiging);

  // Kleine vertraging zodat de browser de pagina kan ontvangen
  delay(2000);
  ESP.restart();
}

// ============================================================
// ROUTE: POST /wissen → factory reset + herstarten
// ============================================================
static void wc_handle_wissen() {
  DBG_INFO("WebConfig: POST /wissen → factory reset");
  bool isEn = (wc_cfg_ptr && strcmp(wc_cfg_ptr->taal, "en") == 0);

  config_wissen();

  String pagina = F(
    "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
    "<style>body{font-family:Arial;background:#0d0d1f;color:#dde;text-align:center;padding:40px}"
    "h1{color:#FF8844}</style></head>"
    "<body><h1>&#128465; "
  );
  pagina += isEn ? "Configuration cleared" : "Configuratie gewist";
  pagina += F("</h1><p>");
  pagina += isEn ? "The device is restarting in setup mode..." : "Het device herstart in instelmodus...";
  pagina += F("</p></body></html>");

  wc_server.send(200, "text/html; charset=utf-8", pagina);
  delay(2000);
  ESP.restart();
}

// ============================================================
// LVGL SCHERM: toon instelmodus instructies op het display
// Gebruikt g_lang (ingesteld vóór webconfig_ap_starten() aanroep)
// ============================================================
static void wc_toon_instelmodus_scherm(lv_obj_t* scherm) {
  lv_obj_set_style_bg_color(scherm, lv_color_hex(0x0A0A2E), 0);
  lv_obj_set_style_bg_opa(scherm, LV_OPA_COVER, 0);

  // Titel (taalafhankelijk via g_lang)
  lv_obj_t* titel = lv_label_create(scherm);
  lv_label_set_text(titel, g_lang->wc_ap_titel);
  lv_obj_set_style_text_font(titel, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(titel, lv_color_hex(0x44AAFF), 0);
  lv_obj_align(titel, LV_ALIGN_TOP_MID, 0, 15);

  // Instructies (taalafhankelijk via g_lang)
  lv_obj_t* info = lv_label_create(scherm);
  lv_label_set_text(info, g_lang->wc_ap_instructie);
  lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(info, lv_color_hex(0xCCDDFF), 0);
  lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(info, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(info, 155);
  lv_obj_align(info, LV_ALIGN_CENTER, 0, 10);

  // Spinner
  lv_obj_t* spinner = lv_spinner_create(scherm);
  lv_obj_set_size(spinner, 24, 24);
  lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, -15);
  lv_obj_set_style_arc_color(spinner, lv_color_hex(0x44AAFF), LV_PART_INDICATOR);
}

// ============================================================
// WEBCONFIG INITIALISEREN (routes registreren + config pointer)
// ============================================================
static void wc_init(AppConfig& cfg) {
  wc_cfg_ptr = &cfg;
  wc_server.on("/",        HTTP_GET,  wc_handle_root);
  wc_server.on("/opslaan", HTTP_POST, wc_handle_opslaan);
  wc_server.on("/wissen",  HTTP_POST, wc_handle_wissen);
  // Captive portal: alle andere paden → root
  wc_server.onNotFound([]() {
    wc_server.sendHeader("Location", "http://192.168.4.1/", true);
    wc_server.send(302, "text/plain", "");
  });
}

// ============================================================
// AP-MODUS STARTEN + EIGEN LOOP (blokkerend tot herstart)
// Roep aan vanuit setup() als geen config gevonden is.
// ============================================================
static void webconfig_ap_starten(AppConfig& cfg) {
  DBG_INFO("WebConfig: AP-modus starten als '%s'", AP_SSID);

  // LVGL instelmodus scherm tonen
  lv_obj_t* scherm = lv_scr_act();
  lv_obj_clean(scherm);
  wc_toon_instelmodus_scherm(scherm);
  lv_task_handler();  // Direct renderen

  // WiFi instellen als Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID);
  delay(200);  // Wacht tot AP volledig gestart is

  // DNS server: alle domeinen → 192.168.4.1 (captive portal)
  wc_dns.start(DNS_PORT, "*", WiFi.softAPIP());
  DBG_INFO("AP IP: %s", WiFi.softAPIP().toString().c_str());

  // WebServer routes registreren
  wc_init(cfg);
  wc_server.begin();
  DBG_INFO("WebConfig server gestart op http://%s/", AP_IP_STR);

  // Eigen loop: LVGL + WebServer + DNS gelijktijdig afhandelen
  while (true) {
    wc_dns.processNextRequest();
    wc_server.handleClient();
    lv_task_handler();
    delay(5);
  }
  // Nooit bereikt: wc_handle_opslaan() roept ESP.restart() aan
}

// ============================================================
// STA-MODUS WEBSERVER STARTEN (na succesvolle WiFi-verbinding)
// Niet-blokkerend: roep webconfig_handleclient() in de hoofdloop aan.
// ============================================================
static void webconfig_sta_starten(AppConfig& cfg) {
  wc_init(cfg);
  wc_server.begin();
  DBG_INFO("WebConfig STA server gestart. Bereikbaar via http://%s/",
           WiFi.localIP().toString().c_str());
}

// ============================================================
// HANDLECLIENT: aanroepen in de LVGL hoofdloop
// ============================================================
static void webconfig_handleclient() {
  wc_server.handleClient();
}

// EINDE SCRIPT
