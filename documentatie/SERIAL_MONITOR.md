# Serial Monitor — Handleiding

**Bestand** : documentatie/SERIAL_MONITOR.md
**Project** : ESP32C6_SMARTDISPLAY
**Datum**   : 2026-03-07 00:14:26 (CET)

---

## Achtergrond

De ESP32-C6 gebruikt **HWCDC** (Hardware USB-CDC) voor seriële communicatie via de ingebouwde USB-JTAG interface (`/dev/ttyACM1`). De ingebouwde PlatformIO VSCode serial monitor werkt hiermee niet correct. Gebruik in plaats daarvan het meegeleverde `monitor.py` script.

---

## Vereisten

Python 3 met pyserial:

```bash
pip install pyserial
```

---

## Gebruik

### Starten

Open een terminal in VSCode (`Ctrl+`` ` ``) en voer uit vanuit de projectmap:

```bash
python3 monitor.py
```

### Stoppen

```
Ctrl+C
```

### Na een reset

Het script herverbindt **automatisch** na elke reset van de ESP. Je ziet dan:

```
--- Verbroken, herverbinden... ---
--- Verbonden ---
===========================================
[BOOT] Versie    : v2.9.1
[BOOT] Datum     : ...
...
```

---

## Debug niveaus

In `src/ESP32C6_SmartDisplay.ino` regel 47–48:

| Instelling | Waarde | Effect |
|---|---|---|
| `#define DEBUG_SERIAL` | `1` | Serial aan / `0` = uit |
| `#define DEBUG_LEVEL` | `1` | Alleen fouten |
| `#define DEBUG_LEVEL` | `2` | Fouten + waarschuwingen |
| `#define DEBUG_LEVEL` | `3` | + info (standaard) |
| `#define DEBUG_LEVEL` | `4` | + verbose (touch events, etc.) |

---

## Tip: uploaden met monitor open

Stop de monitor **eerst** (`Ctrl+C`) voor je een nieuwe firmware uploadt. Als de monitor de poort bezet houdt, mislukt de upload met een `serial exception` fout.

Na de upload start je de monitor opnieuw.

---

## Poortnummer wijzigen

Als de ESP op een andere poort verschijnt (bijv. `/dev/ttyACM0`), pas dan regel 14 aan in `monitor.py`:

```python
PORT = '/dev/ttyACM0'
```
