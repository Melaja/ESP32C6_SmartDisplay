/*
  Bestand : ESP32C6_SmartDisplay.ino
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Multifunctioneel dashboard voor de Waveshare ESP32-C6-Touch-LCD-1.47.
            Drie interactieve touchschermen via LVGL v9:
              1. Datum/Tijd     : Datum in het Nederlands + tijd via NTP
              2. Zoek Telefoon  : Telegram-bericht sturen om telefoon te vinden
              3. WiFi Scanner   : Scan WiFi-netwerken + gedetailleerde info

            Configuratieportaal:
              Bij eerste opstart of mislukte WiFi-verbinding start het device
              als "ESP32-Config" accesspoint. Verbind met dat WiFi en open
              192.168.4.1 in uw browser om WiFi, Telegram en tijdzone in te stellen.
              Na opslaan herstart het device automatisch.
              Nadien bereikbaar via http://[IP-adres]/ op uw eigen WiFi.

  Auteur  : JWP van Renen
  Versie  : v2.2.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)

  Hardware:
    Bord           : Waveshare ESP32-C6-Touch-LCD-1.47
    Chip           : ESP32-C6FH8 (RISC-V, 160MHz, 8MB Flash, 512KB SRAM)
    Display        : 1.47" IPS, 172x320px, JD9853 controller, SPI
    Touch          : Capacitief, AXS5106L controller, I2C (adres 0x63)
    Connectiviteit : WiFi 6 (802.11ax), Bluetooth 5 BLE

  Bibliotheekversies (verplicht exact - alle LOKAAL in libraries/):
    - lvgl                   : v9.5.0 (lokaal in libraries/lvgl/)
    - GFX_Library_for_Arduino: v1.5.9 (lokaal in libraries/GFX_Library_for_Arduino/)
    - esp_lcd_touch_axs5106l : v1.0   (lokaal in libraries/esp_lcd_touch_axs5106l/)
    - esp32 board package    : espressif32 v3.0.0+ (PlatformIO platform)

  PlatformIO bordinstellingen (zie platformio.ini):
    Board             : esp32-c6-devkitc-1
    CPU Frequency     : 160 MHz
    Flash Size        : 8MB
    Upload Speed      : 921600
    lib_dir           : libraries (alle dependencies lokaal)

  Pin configuratie: zie libraries/Mylibrary/pin_config.h
*/

// ============================================================
// DEBUG SCHAKELAAR
// ============================================================
#define DEBUG_SERIAL 1
#define DEBUG_LEVEL 3
#define VERSIE_STRING "v2.4.0"

// ============================================================
// VERPLICHTE INCLUDE VOLGORDE
// ============================================================
#include <Arduino.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <esp_lcd_touch_axs5106l.h>

// WiFi en tijd
#include <WiFi.h>
#include <time.h>
#include <esp_sntp.h>

// BLE
#include <BLEDevice.h>

// Project-eigen bestanden
#include "pin_config.h"
#include "wifi_config.h"
#include "debug_log.h"
#include "config_manager.h"
#include "lang.h"
#include "web_config.h"

// Scherm-modules
#include "screen_datetime.h"
#include "screen_phonefinder.h"
#include "screen_wifiscanner.h"

// ============================================================
// GLOBALE CONFIGURATIE EN TAAL
// (toegankelijk vanuit alle schermen via extern)
// ============================================================
AppConfig g_config;
const Lang* g_lang = &LANG_NL;  // standaard Nederlands; wordt na config laden ingesteld

// ============================================================
// GLOBALE CONSTANTEN
// ============================================================
static const uint32_t SERIAL_BAUD_RATE   = 115200;
static const uint32_t LVGL_BUFFER_RIJEN  = 40;
static const uint8_t  BL_LEDC_KANAAL     = 0;
static const uint32_t BL_LEDC_FREQ_HZ    = 5000;
static const uint8_t  BL_LEDC_RESOLUTIE  = 10;
static const uint32_t BL_HELDERHEID      = 1023;
static const uint8_t  TAB_DATETIME       = 0;
static const uint8_t  TAB_PHONEFINDER    = 1;
static const uint8_t  TAB_WIFISCANNER    = 2;
static const uint8_t  WIFI_MAX_POGINGEN  = 30;
static const uint32_t WIFI_WACHT_MS      = 1000;

// ============================================================
// GLOBALE OBJECT POINTERS
// ============================================================
static Arduino_DataBus* display_bus  = nullptr;
static Arduino_GFX*     gfx         = nullptr;
static lv_display_t*    lvgl_display = nullptr;
static lv_color_t*      lvgl_buffer  = nullptr;
static lv_obj_t*        hoofd_tabview= nullptr;

// ============================================================
// DISPLAY REGISTER INITIALISATIE (JD9853)
// ============================================================
static void display_register_init(void) {
  static const uint8_t init_seq[] = {
    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x11,
    END_WRITE,
    DELAY, 120,

    BEGIN_WRITE,
    WRITE_C8_D16, 0xDF, 0x98, 0x53,
    WRITE_C8_D8,  0xB2, 0x23,

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 4,
    0x00, 0x47, 0x00, 0x6F,

    WRITE_COMMAND_8, 0xBB,
    WRITE_BYTES, 6,
    0x1C, 0x1A, 0x55, 0x73, 0x63, 0xF0,

    WRITE_C8_D16, 0xC0, 0x44, 0xA4,
    WRITE_C8_D8,  0xC1, 0x16,

    WRITE_COMMAND_8, 0xC3,
    WRITE_BYTES, 8,
    0x7D, 0x07, 0x14, 0x06, 0xCF, 0x71, 0x72, 0x77,

    WRITE_COMMAND_8, 0xC4,
    WRITE_BYTES, 12,
    0x00, 0x00, 0xA0, 0x79, 0x0B, 0x0A, 0x16,
    0x79, 0x0B, 0x0A, 0x16, 0x82,

    WRITE_COMMAND_8, 0xC8,
    WRITE_BYTES, 32,
    0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28,
    0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,
    0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28,
    0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,

    WRITE_COMMAND_8, 0xD0,
    WRITE_BYTES, 5,
    0x04, 0x06, 0x6B, 0x0F, 0x00,

    WRITE_C8_D16, 0xD7, 0x00, 0x30,
    WRITE_C8_D8,  0xE6, 0x14,
    WRITE_C8_D8,  0xDE, 0x01,

    WRITE_COMMAND_8, 0xB7,
    WRITE_BYTES, 5,
    0x03, 0x13, 0xEF, 0x35, 0x35,

    WRITE_COMMAND_8, 0xC1,
    WRITE_BYTES, 3,
    0x14, 0x15, 0xC0,

    WRITE_C8_D16, 0xC2, 0x06, 0x3A,
    WRITE_C8_D16, 0xC4, 0x72, 0x12,
    WRITE_C8_D8,  0xBE, 0x00,
    WRITE_C8_D8,  0xDE, 0x02,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3, 0x00, 0x02, 0x00,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3, 0x01, 0x02, 0x00,

    WRITE_C8_D8,  0xDE, 0x00,
    WRITE_C8_D8,  0x35, 0x00,
    WRITE_C8_D8,  0x3A, 0x05,

    WRITE_COMMAND_8, 0x2A,
    WRITE_BYTES, 4,
    0x00, (uint8_t)LCD_COL_OFFSET,
    0x00, (uint8_t)(LCD_COL_OFFSET + LCD_BREEDTE - 1),

    WRITE_COMMAND_8, 0x2B,
    WRITE_BYTES, 4,
    0x00, 0x00, 0x01, 0x3F,

    WRITE_C8_D8,  0xDE, 0x02,

    WRITE_COMMAND_8, 0xE5,
    WRITE_BYTES, 3, 0x00, 0x02, 0x00,

    WRITE_C8_D8,  0xDE, 0x00,
    WRITE_C8_D8,  0x36, 0x00,
    WRITE_COMMAND_8, 0x21,
    END_WRITE,
    DELAY, 10,

    BEGIN_WRITE,
    WRITE_COMMAND_8, 0x29,
    END_WRITE
  };

  display_bus->batchOperation(init_seq, sizeof(init_seq));
  DBG_INFO("JD9853 register initialisatie uitgevoerd.");
}

// ============================================================
// LVGL FLUSH CALLBACK
// ============================================================
static void lvgl_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  uint32_t breedte = lv_area_get_width(area);
  uint32_t hoogte  = lv_area_get_height(area);
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)px_map, breedte, hoogte);
  lv_disp_flush_ready(disp);
}

// ============================================================
// TOUCH CALLBACK
// ============================================================
static void lvgl_touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
  LV_UNUSED(indev);
  bsp_touch_read();
  touch_data_t touch_info;
  if (bsp_touch_get_coordinates(&touch_info) && touch_info.touch_num > 0) {
    data->state   = LV_INDEV_STATE_PRESSED;
    data->point.x = touch_info.coords[0].x;
    data->point.y = touch_info.coords[0].y;
    DBG_VERBOSE("Touch: x=%d, y=%d", data->point.x, data->point.y);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ============================================================
// LVGL TICK CALLBACK
// ============================================================
static uint32_t lvgl_tick_cb(void) {
  return millis();
}

// ============================================================
// DISPLAY INITIALISATIE
// ============================================================
static void display_initialiseren(void) {
  DBG_INFO("Display initialisatie starten...");

  display_bus = new Arduino_HWSPI(LCD_DC_PIN, LCD_CS_PIN, LCD_SCK_PIN, LCD_MOSI_PIN);
  gfx = new Arduino_ST7789(
    display_bus, LCD_RST_PIN, 0, false,
    LCD_BREEDTE, LCD_HOOGTE,
    LCD_COL_OFFSET, LCD_ROW_OFFSET,
    LCD_COL_OFFSET, LCD_ROW_OFFSET
  );

  if (!gfx->begin(LCD_SPI_FREQ_HZ)) {
    DBG_ERROR("gfx->begin() mislukt! Reboot over 5 seconden...");
    delay(5000);
    ESP.restart();
    return;
  }

  display_register_init();
  gfx->fillScreen(RGB565_BLACK);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LCD_BL_PIN, BL_LEDC_FREQ_HZ, BL_LEDC_RESOLUTIE);
  ledcWrite(LCD_BL_PIN, BL_HELDERHEID);
#else
  ledcSetup(BL_LEDC_KANAAL, BL_LEDC_FREQ_HZ, BL_LEDC_RESOLUTIE);
  ledcAttachPin(LCD_BL_PIN, BL_LEDC_KANAAL);
  ledcWrite(BL_LEDC_KANAAL, BL_HELDERHEID);
#endif

  DBG_INFO("Display initialisatie geslaagd. Resolutie: %dx%d", LCD_BREEDTE, LCD_HOOGTE);
}

// ============================================================
// LVGL INITIALISATIE
// ============================================================
static void lvgl_initialiseren(void) {
  DBG_INFO("LVGL initialisatie starten...");
  lv_init();
  lv_tick_set_cb(lvgl_tick_cb);

  uint32_t buffer_grootte = LCD_BREEDTE * LVGL_BUFFER_RIJEN;
  lvgl_buffer = (lv_color_t*)heap_caps_malloc(
    buffer_grootte * sizeof(lv_color_t),
    MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT
  );
  if (lvgl_buffer == NULL) {
    DBG_WARN("LVGL buffer in intern SRAM mislukt, probeer extern geheugen...");
    lvgl_buffer = (lv_color_t*)heap_caps_malloc(
      buffer_grootte * sizeof(lv_color_t), MALLOC_CAP_8BIT
    );
  }
  if (lvgl_buffer == NULL) {
    DBG_ERROR("LVGL draw buffer allocatie mislukt!");
    delay(5000);
    ESP.restart();
    return;
  }

  lvgl_display = lv_display_create(LCD_BREEDTE, LCD_HOOGTE);
  lv_display_set_flush_cb(lvgl_display, lvgl_flush_cb);
  lv_display_set_buffers(
    lvgl_display, lvgl_buffer, NULL,
    buffer_grootte * sizeof(lv_color_t),
    LV_DISPLAY_RENDER_MODE_PARTIAL
  );

  Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_I2C_FREQ);
  bsp_touch_init(&Wire, TOUCH_RST_PIN, TOUCH_INT_PIN,
                 gfx->getRotation(), LCD_BREEDTE, LCD_HOOGTE);

  lv_indev_t* touch_indev = lv_indev_create();
  lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touch_indev, lvgl_touch_read_cb);

  DBG_INFO("LVGL initialisatie geslaagd.");
}

// ============================================================
// WIFI VERBINDEN (met config-parameters)
// ============================================================
static bool wifi_verbinden(const char* ssid, const char* wachtwoord) {
  DBG_INFO("WiFi verbinden met '%s'...", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wachtwoord);

  uint8_t pogingen = 0;
  while (WiFi.status() != WL_CONNECTED && pogingen < WIFI_MAX_POGINGEN) {
    delay(WIFI_WACHT_MS);
    pogingen++;
    DBG_INFO("WiFi poging %d/%d...", pogingen, WIFI_MAX_POGINGEN);
    lv_task_handler();  // LVGL blijft responsieven tijdens wachten
  }

  if (WiFi.status() == WL_CONNECTED) {
    DBG_INFO("WiFi verbonden! IP: %s, RSSI: %d dBm",
             WiFi.localIP().toString().c_str(), WiFi.RSSI());
    // NTP starten met tijdzone uit config
    configTzTime(g_config.ntp_tijdzone, NTP_SERVER_1, NTP_SERVER_2);
    DBG_INFO("NTP gestart (TZ: %s)", g_config.ntp_tijdzone);
    return true;
  } else {
    DBG_WARN("WiFi verbinding mislukt na %d pogingen.", WIFI_MAX_POGINGEN);
    return false;
  }
}

// ============================================================
// UI AANMAKEN
// ============================================================
static void ui_aanmaken(void) {
  DBG_INFO("UI opbouwen...");

  hoofd_tabview = lv_tabview_create(lv_scr_act());
  lv_tabview_set_tab_bar_position(hoofd_tabview, LV_DIR_BOTTOM);
  lv_tabview_set_tab_bar_size(hoofd_tabview, 42);
  lv_obj_set_style_bg_color(
    lv_tabview_get_tab_bar(hoofd_tabview), lv_color_hex(0x111122), 0);
  lv_obj_set_style_pad_hor(lv_tabview_get_tab_bar(hoofd_tabview), 12, 0);

  lv_obj_t* tab_datetime    = lv_tabview_add_tab(hoofd_tabview, LV_SYMBOL_HOME);
  lv_obj_t* tab_phonefinder = lv_tabview_add_tab(hoofd_tabview, LV_SYMBOL_CALL);
  lv_obj_t* tab_wifiscanner = lv_tabview_add_tab(hoofd_tabview, LV_SYMBOL_WIFI);

  scherm_datetime_aanmaken(tab_datetime);
  scherm_phonefinder_aanmaken(tab_phonefinder);
  scherm_wifiscanner_aanmaken(tab_wifiscanner);

  lv_tabview_set_active(hoofd_tabview, TAB_DATETIME, LV_ANIM_OFF);

  DBG_INFO("UI opgebouwd: 3 tabbladen aangemaakt.");
}

// ============================================================
// SETUP (Arduino entrypoint)
// ============================================================
void setup() {
  // Stap 1: Serial debug
  if (DEBUG_SERIAL) {
    Serial.begin(SERIAL_BAUD_RATE);
    uint32_t start_ms = millis();
    while (!Serial && millis() - start_ms < 2000) delay(10);
  }
  dbg_boot_info(VERSIE_STRING);

  // Stap 2: Display + LVGL initialiseren
  display_initialiseren();
  lvgl_initialiseren();
  DBG_INFO("Vrij heap na display+LVGL: %u bytes", esp_get_free_heap_size());

  // Stap 3: Configuratie laden uit NVS
  bool geconfigureerd = config_laden(g_config);

  // Taal instellen op basis van config (vóór UI opbouwen)
  g_lang = (strcmp(g_config.taal, "en") == 0) ? &LANG_EN : &LANG_NL;
  DBG_INFO("Interface taal: %s", g_config.taal);

  // Stap 4: WiFi verbinden of AP-configuratiemodus starten
  bool wifi_ok = false;
  if (geconfigureerd && g_config.wifi_ssid[0] != '\0') {
    wifi_ok = wifi_verbinden(g_config.wifi_ssid, g_config.wifi_wachtwoord);
  }

  if (!wifi_ok) {
    // Geen config of WiFi mislukt → AP-modus configuratieportaal
    // webconfig_ap_starten() heeft een eigen eindeloze loop en roept
    // ESP.restart() aan nadat de config is opgeslagen.
    DBG_INFO("Starten in configuratiemodus (AP)...");
    webconfig_ap_starten(g_config);
    return;  // Nooit bereikt
  }

  // Stap 5: BLE stack initialiseren
  BLEDevice::init("ESP32C6_SmartDisplay");
  DBG_INFO("BLE stack geïnitialiseerd.");

  // Stap 6: WebServer starten in STA-modus (bereikbaar via http://[IP]/)
  webconfig_sta_starten(g_config);
  DBG_INFO("Config webserver bereikbaar op http://%s/",
           WiFi.localIP().toString().c_str());

  // Stap 7: UI aanmaken
  ui_aanmaken();

  DBG_INFO("=== Setup voltooid (%lu ms) ===", millis());
  DBG_INFO("Vrij heap na UI: %u bytes", esp_get_free_heap_size());
}

// ============================================================
// LOOP (Arduino hoofdlus)
// ============================================================
void loop() {
  uint32_t wacht_ms = lv_task_handler();

  // WebServer verzoeken afhandelen (config via browser in STA-modus)
  webconfig_handleclient();

  if (wacht_ms > 5) wacht_ms = 5;
  delay(wacht_ms);
}

// EINDE SCRIPT
