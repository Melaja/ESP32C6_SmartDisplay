/**
 * @file lv_conf.h
 * LVGL v9.5.0 configuratie voor ESP32C6_SmartDisplay
 * Waveshare ESP32-C6-Touch-LCD-1.47 (172x320, JD9853, LVGL v9.5.0)
 *
 * Gebaseerd op het officiële lv_conf_template.h van LVGL v9.5.0.
 * Alleen de voor dit project relevante instellingen zijn gewijzigd.
 *
 * Project : ESP32C6_SMARTDISPLAY
 * Auteur  : JWP van Renen
 * Versie  : v1.0.0
 * Datum   : 2026-03-04 00:00:00 (Europe/Brussels)
 */

/* clang-format off */
#if 1  /* Zet op "1" om de inhoud te activeren */

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
   KLEURDIEPTE
 *====================*/
/** Kleurdiepte: 16 bits = RGB565
    Reden: JD9853 display communiceert via SPI in 16-bit RGB565 formaat.
    262K kleuren worden efficiënt aangeboden. */
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER INSTELLINGEN
 *=========================*/
/** Gebruik LVGL's ingebouwde geheugenallocator.
    Reden: ESP32 Arduino omgeving heeft geen speciale malloc vereisten.
    LV_STDLIB_BUILTIN is stabiel en goed geoptimaliseerd voor embedded. */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

/* Standaard C-header includes (vereist door LVGL v9) */
#define LV_STDINT_INCLUDE       <stdint.h>
#define LV_STDDEF_INCLUDE       <stddef.h>
#define LV_STDBOOL_INCLUDE      <stdbool.h>
#define LV_INTTYPES_INCLUDE     <inttypes.h>
#define LV_LIMITS_INCLUDE       <limits.h>
#define LV_STDARG_INCLUDE       <stdarg.h>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** LVGL intern geheugenblok: 48KB
        Reden: Voldoende voor 3 schermen met widgets, zonder te veel
        SRAM te bezetten. WiFi stack (~20KB) en BLE stack (~30KB)
        verbruiken ook SRAM. ESP32-C6 heeft 512KB HP SRAM totaal. */
    #define LV_MEM_SIZE (48 * 1024U)
    #define LV_MEM_POOL_EXPAND_SIZE 0
    #define LV_MEM_ADR 0
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC
    #endif
#endif

/*====================
   HAL INSTELLINGEN
 *====================*/
/** Verversingsperiode: 33ms = 30 fps. Voldoende voor dashboard. */
#define LV_DEF_REFR_PERIOD  33      /* [ms] */

/** DPI voor het 1.47" display: 172px / 1.47" = ~117 DPI, afgerond naar 120 */
#define LV_DPI_DEF 120              /* [px/inch] */

/*=================
 * OPERATING SYSTEM
 *=================*/
/** Geen RTOS: ESP32 Arduino loop() is enkelvoudige taak. */
#define LV_USE_OS   LV_OS_NONE

/*========================
 * RENDERING CONFIGURATIE
 *========================*/
#define LV_DRAW_BUF_STRIDE_ALIGN    1
#define LV_DRAW_BUF_ALIGN           4
#define LV_DRAW_TRANSFORM_USE_MATRIX 0

/** Buffer voor tijdelijke render-lagen: 24KB */
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /* [bytes] */
#define LV_DRAW_LAYER_MAX_MEMORY 0
#define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /* [bytes] */
#define LV_DRAW_THREAD_PRIO LV_THREAD_PRIO_HIGH

/** Software rendering (vereist, geen hardware GPU op ESP32-C6) */
#define LV_USE_DRAW_SW 1
#if LV_USE_DRAW_SW == 1
    #define LV_DRAW_SW_SUPPORT_RGB565               1
    #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED       1
    #define LV_DRAW_SW_SUPPORT_RGB565A8             1
    #define LV_DRAW_SW_SUPPORT_RGB888               1
    #define LV_DRAW_SW_SUPPORT_XRGB8888             1
    #define LV_DRAW_SW_SUPPORT_ARGB8888             1
    #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1
    #define LV_DRAW_SW_SUPPORT_L8                   0
    #define LV_DRAW_SW_SUPPORT_AL88                 0
    #define LV_DRAW_SW_SUPPORT_A8                   1
    #define LV_DRAW_SW_SUPPORT_I1                   0
    #define LV_DRAW_SW_I1_LUM_THRESHOLD 127
    #define LV_DRAW_SW_DRAW_UNIT_CNT    1
    #define LV_USE_DRAW_ARM2D_SYNC      0
#endif

/** Externe GPU-versnellers: uitgeschakeld (niet aanwezig op ESP32-C6) */
#define LV_USE_NEMA_GFX      0
#define LV_USE_PXP           0
#define LV_USE_G2D           0
#define LV_USE_DRAW_DAVE2D   0
#define LV_USE_DRAW_SDL      0
#define LV_USE_DRAW_VG_LITE  0
#define LV_USE_DRAW_DMA2D    0
#define LV_USE_DRAW_OPENGLES 0
#define LV_USE_PPA           0
#define LV_USE_DRAW_EVE      0
#define LV_USE_DRAW_NANOVG   0

/*====================
   LOGGING
 *====================*/
#define LV_USE_LOG 0
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0
#define LV_LOG_USE_TIMESTAMP 0
#define LV_LOG_USE_FILE_LINE 1

/*====================
   ASSERT CONTROLES
 *====================*/
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);

/*====================
   DEBUG MONITORS
 *====================*/
#define LV_USE_REFR_DEBUG          0
#define LV_USE_LAYER_DEBUG         0
#define LV_USE_PARALLEL_DRAW_DEBUG 0

/*====================
   GLOBALE INSTELLINGEN
 *====================*/
#define LV_ENABLE_GLOBAL_CUSTOM 0
#define LV_CACHE_DEF_SIZE            0
#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0
#define LV_GRADIENT_MAX_STOPS        2
#define LV_COLOR_MIX_ROUND_OFS       0
#define LV_OBJ_STYLE_CACHE           0
#define LV_USE_OBJ_ID                0
#define LV_USE_OBJ_NAME              0
#define LV_OBJ_ID_AUTO_ASSIGN        LV_USE_OBJ_ID
#define LV_USE_OBJ_ID_BUILTIN        1
#define LV_USE_OBJ_PROPERTY          0
#define LV_USE_OBJ_PROPERTY_NAME     1
#define LV_USE_GESTURE_RECOGNITION   0
#define LV_BIG_ENDIAN_SYSTEM         0
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE  1
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_FAST_MEM
#define LV_ATTRIBUTE_DMA
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#define LV_USE_LARGE_COORD 0

/*====================
   INGEBOUWDE LETTERTYPEN (MONTSERRAT)
 *====================*/
/** Ingeschakeld zijn de maten die in ESP32C6_SmartDisplay gebruikt worden:
    12pt=status labels, 14pt=standaard tekst, 16pt=datum/tabbladen,
    20pt=SSID/apparaatnamen, 24pt=grote datum, 32pt=tijdsweergave */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/** Standaard lettertype: 14pt Montserrat */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/** Speciale lettertypen: uitgeschakeld */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUN_16_CJK            0
#define LV_FONT_UNSCII_8                 0
#define LV_FONT_UNSCII_16                0

/*====================
   TEKST INSTELLINGEN
 *====================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_"
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*====================
   WIDGETS
 *====================*/
#define LV_USE_ANIMIMG    0
#define LV_USE_ARC        1   /* Halve/hele cirkel: voor BLE signaalweergave */
#define LV_USE_ARCLABEL   1
#define LV_USE_BAR        1   /* Voortgangsbalk: voor RSSI visualisatie */
#define LV_USE_BTN        1   /* Knoppen: Scan, Terug etc. */
#define LV_USE_BTNMATRIX  0
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     0
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_COLORWHEEL 0
#define LV_USE_DROPDOWN   1   /* Keuzelijst */
#define LV_USE_IMG        1
#define LV_USE_IMAGEBUTTON 0
#define LV_USE_KEYBOARD   0
#define LV_USE_LABEL      1   /* Tekstlabels: essentieel */
#define LV_USE_LED        0
#define LV_USE_LINE       0
#define LV_USE_LIST       1   /* Scrollbare lijst: BLE en WiFi netwerken */
#define LV_USE_MENU       0
#define LV_USE_MSGBOX     1   /* Berichtvenster: voor foutmeldingen */
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      0
#define LV_USE_SLIDER     0
#define LV_USE_SPANGROUP  0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    1   /* Laadanimatie: tijdens scan */
#define LV_USE_SWITCH     0
#define LV_USE_TABLE      1   /* Tabel: voor WiFi details */
#define LV_USE_TABVIEW    1   /* Tabbladen: voor 3 hoofdschermen */
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0

/*====================
   THEMA
 *====================*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 1              /* Donker thema */
    #define LV_THEME_DEFAULT_GROW 1              /* Knop groei-effect bij aanraken */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80  /* ms voor stijlovergangen */
#endif
#define LV_USE_THEME_SIMPLE 0
#define LV_USE_THEME_MONO   0

/*====================
   LAYOUTS
 *====================*/
#define LV_USE_FLEX 1   /* Flex layout: nodig voor lijstweergaven */
#define LV_USE_GRID 0

/*====================
   EXTRA COMPONENTEN (allen uitgeschakeld)
 *====================*/
#define LV_USE_SNAPSHOT    0
#define LV_USE_SYSMON      0
#define LV_USE_PROFILER    0
#define LV_USE_MONKEY      0
#define LV_USE_GRIDNAV     0
#define LV_USE_FRAGMENT    0
#define LV_USE_IMGFONT     0
#define LV_USE_TRANSLATION 0
#define LV_USE_GESTURE_MANAGER 0
#define LV_USE_OBSERVER    0
#define LV_USE_BARCODE     0
#define LV_USE_QRCODE      0
#define LV_USE_RLOTTIE     0
#define LV_USE_FFMPEG      0
#define LV_USE_FREETYPE    0
#define LV_USE_TINY_TTF    0
#define LV_USE_THORVG      0
#define LV_USE_LZ4         0
#define LV_USE_BMP         0
#define LV_USE_GIF         0
#define LV_USE_LODEPNG     0
#define LV_USE_LIBPNG      0
#define LV_USE_SJPG        0
#define LV_USE_LIBJPEG_TURBO 0
#define LV_USE_TJPGD       0
#define LV_USE_NUTTX       0

/*====================
   DEMOS (UITGESCHAKELD IN PRODUCTIE)
 *====================*/
#define LV_USE_DEMO_WIDGETS            0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK          0
#define LV_USE_DEMO_RENDER             0
#define LV_USE_DEMO_STRESS             0
#define LV_USE_DEMO_MUSIC              0
#define LV_USE_DEMO_FLEX_LAYOUT        0
#define LV_USE_DEMO_MULTILANG          0
#define LV_USE_DEMO_TRANSFORM          0
#define LV_USE_DEMO_SCROLL             0
#define LV_USE_DEMO_VECTOR_GRAPHIC     0

#endif /* LV_CONF_H */
#endif /* End of "#if 1" */

// EINDE SCRIPT
