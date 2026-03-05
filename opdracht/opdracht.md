# Opdracht: ESP32C6_SmartDisplay

## Hardware
Waveshare ESP32-C6-Touch-LCD-1.47
- Chip: ESP32-C6FH8
- Display: 1.47" IPS, 172×320, JD9853, SPI
- Touch: AXS5106L, I2C (adres 0x63)
- WiFi 6 (2.4GHz), Bluetooth 5 BLE

## Functionele Eisen

### 1. Datum/Tijd Scherm (REQ-001 t/m REQ-003)
- Toont huidige datum in Nederlands formaat: "07 mei 1955 zaterdag"
- Toont huidige tijd in 24-uurs formaat: "23:59:59"
- Tijd gesynchroniseerd via NTP over WiFi
- Automatische zomertijd (tijdzone Europe/Brussels)

### 2. Telefoon Zoeker Scherm (REQ-004 t/m REQ-007)
- BLE-scan van omgeving
- Lijst van gevonden apparaten met signaalsterkte (RSSI in dBm)
- Gesorteerd op signaalsterkte (sterkste = dichtste bij)
- Kleurgecodeerde signaalbalken voor visuele nabijheid

### 3. WiFi Scanner Scherm (REQ-008 t/m REQ-012)
- Scant beschikbare WiFi-netwerken
- Lijst van gevonden netwerken
- Detailpagina na klik op SSID:
  - SSID naam
  - BSSID (MAC-adres)
  - Kanaal en frequentie
  - Signaalsterkte (RSSI)
  - Beveiligingstype

## Niet-Functionele Eisen

### Technisch
- Alle code in LVGL v9 (laatste versie)
- Volgens de interne projectcode-standaard
- Uitgebreid Nederlands commentaar
- Schakelbare debug output (DEBUG_SERIAL)
- Lokale libraries in /libraries/

### Kwaliteit
- Geen magische getallen
- Fail-safe gedrag voor WiFi, BLE, display
- Versie v1.0.0 bij boot gelogd

## Status: v1.0.0 - Initieel opgeleverd (2026-03-04)
