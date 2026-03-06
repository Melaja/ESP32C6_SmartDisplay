#!/usr/bin/env python3
"""
Bestand : monitor.py
Project : ESP32C6_SMARTDISPLAY
Doel    : Serial monitor voor ESP32-C6 via USB CDC (HWCDC).
          Herverbindt automatisch na reset van de ESP.
          Gebruik: python3 monitor.py
          Stoppen: Ctrl+C
Auteur  : JWP van Renen
Versie  : v1.0.0
Datum   : 2026-03-07 00:13:01 (CET)
"""

import serial
import sys
import time

PORT  = '/dev/ttyACM1'
BAUD  = 115200

print(f"Serial monitor op {PORT} @ {BAUD} baud")
print("Stoppen met Ctrl+C")
print("=" * 40)

while True:
    try:
        s = serial.Serial(PORT, BAUD, timeout=0.5)
        print("--- Verbonden ---", flush=True)
        while True:
            d = s.read(512)
            if d:
                sys.stdout.buffer.write(d)
                sys.stdout.flush()
    except serial.SerialException:
        print("--- Verbroken, herverbinden... ---", flush=True)
        try:
            s.close()
        except Exception:
            pass
        time.sleep(1)
    except KeyboardInterrupt:
        print("\n--- Gestopt ---")
        break
