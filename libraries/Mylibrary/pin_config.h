/*
  Bestand : pin_config.h
  Project : ESP32C6_SMARTDISPLAY
  Doel    : Centrale definitie van alle hardware-pinnen voor de
            Waveshare ESP32-C6-Touch-LCD-1.47 ontwikkelbaard.
            Alle pin-nummers zijn gebaseerd op de officiële Waveshare
            demo (ESP32-C6-Touch-LCD-1.47-Demo.zip, bsp_display.h / bsp_touch.h).
  Auteur  : JWP van Renen
  Versie  : v1.0.0
  Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
*/

#pragma once

// ============================================================
// DISPLAY INTERFACE - SPI (JD9853 controller)
// Aansluitingen op de Waveshare ESP32-C6-Touch-LCD-1.47:
//   Scherm resolutie : 172 x 320 pixels (IPS, 262K kleuren)
//   Communicatie     : 4-wire Hardware SPI
//   Controller       : JD9853 (aangestuurd via ST7789-compatibele commando's)
//   SPI frequentie   : 80 MHz (maximaal voor stabiele werking)
// ============================================================

// LCD_SCK: SPI klokpin (Serial Clock)
// Reden: GPIO1 is de door Waveshare vastgelegde SPI-CLK pin voor dit bord
#define LCD_SCK_PIN     1

// LCD_MOSI: SPI datapin (Master Out Slave In = data naar display)
// Reden: GPIO2 is de door Waveshare vastgelegde SPI-MOSI pin voor dit bord
#define LCD_MOSI_PIN    2

// LCD_CS: Chip Select – activeert het display op de SPI-bus (actief laag)
// Reden: GPIO14 is de door Waveshare vastgelegde CS pin
#define LCD_CS_PIN      14

// LCD_DC: Data/Command selectie
//   LOW  = commando naar displaycontroller
//   HIGH = pixeldata naar displaygeheugen
// Reden: GPIO15 is de door Waveshare vastgelegde DC pin
#define LCD_DC_PIN      15

// LCD_RST: Hardware reset van het display (actief laag, minimaal 10ms puls)
// Reden: GPIO22 is de door Waveshare vastgelegde RST pin
#define LCD_RST_PIN     22

// LCD_BL: Achtergrondverlichting (PWM gestuurd via LEDC, hoog = aan)
// Reden: GPIO23 is de door Waveshare vastgelegde BL pin
// PWM frequentie: 5 kHz, 10-bit resolutie (0-1023)
#define LCD_BL_PIN      23

// Display kolom-offset: het JD9853 scherm heeft een offset van 34 pixels
// Dit is nodig omdat de fysieke display-controller meer pixels heeft dan
// het zichtbare gebied van 172 pixels breed.
#define LCD_COL_OFFSET  34
#define LCD_ROW_OFFSET  0

// Schermresolutie (portrait oriëntatie is standaard)
#define LCD_BREEDTE     172
#define LCD_HOOGTE      320

// SPI kloksnelheid: 80 MHz voor maximale refresh rate
// Bij instabiel beeld verlagen naar 40 MHz
#define LCD_SPI_FREQ_HZ (80 * 1000 * 1000)

// ============================================================
// TOUCH INTERFACE - I2C (AXS5106L controller)
// Aansluitingen op de Waveshare ESP32-C6-Touch-LCD-1.47:
//   Touch type       : Capacitief (1 punt)
//   Communicatie     : I2C
//   Controller       : AXS5106L
//   I2C adres        : 0x63 (vast, niet instelbaar op dit bord)
// ============================================================

// TOUCH_SDA: I2C data lijn voor aanraakcontroller
// Reden: GPIO18 is de door Waveshare vastgelegde I2C-SDA pin voor touch
#define TOUCH_SDA_PIN   18

// TOUCH_SCL: I2C klok lijn voor aanraakcontroller
// Reden: GPIO19 is de door Waveshare vastgelegde I2C-SCL pin voor touch
#define TOUCH_SCL_PIN   19

// TOUCH_RST: Hardware reset van de touch-controller (actief laag)
// Initialisatieprocedure: 200ms laag, dan 300ms hoog (per datasheet AXS5106L)
#define TOUCH_RST_PIN   20

// TOUCH_INT: Interrupt van touch-controller naar ESP32
// Stuurt een FALLING flank als aanraking gedetecteerd wordt
// Gebruikt attachInterrupt() voor lage latentie touch-respons
#define TOUCH_INT_PIN   21

// I2C adres van de AXS5106L touch-controller
// Bevestigd via esp_lcd_touch_axs5106l bibliotheek (definitie AXS5106L_ADDR)
#define TOUCH_I2C_ADRES 0x63

// I2C kloksnelheid: 400 kHz (Fast Mode) voor responsieve touch
#define TOUCH_I2C_FREQ  400000

// EINDE SCRIPT
