# Time Module

## Purpose
Provides accurate date/time via DS3231 RTC for timestamping log entries and display.

## Hardware
- **Module:** DS3231 RTC (I²C)
- **Interface:** I²C (shared with AHT20, BMP280)
- **Address:** 0x68 (from `config.h`)
- **Pins:** SDA A4, SCL A5 (shared I²C bus)
- **Power:** 3.3 V–5 V (module typically has onboard 3.3 V regulator and battery backup for coin cell) — **verify your specific module's voltage tolerance**
- **Library (Planned):** `RTClib` (Adafruit fork)

## Pin Assignments (from config.h, verified against pinmap.md)
- I²C SDA: A4, SCL: A5 (shared with AHT20+BMP280)
- **Power:** DS3231 may be 5 V tolerant (many modules include a regulator). Verify your specific module's datasheet. If not 5 V tolerant, it also requires level shifting via the SmartElex converter.

## Planned Responsibilities
- **Initialisation** — Detect DS3231, verify oscillator running, check battery flag
- **Time sync** — Set from compile-time `__DATE__` `__TIME__` on first boot; optional NTP/manual later
- **Time reading** — Populate `currentTime` struct (UTC) on each `loopRTC()` call
- **Alarm/wake** — Configure DS3231 alarms for low-power wake (future, if power module implements sleep)
- **Drift monitoring** — Log temperature-compensated drift stats (DS3231 has ±2 ppm)

## Current Status
**STUBBED** — Functions exist in `abr_firmware.ino` but return false / do nothing; data structure is IMPLEMENTED.

## Implementation Status
| Function / Data Structure | Status | Description |
|---------------------------|--------|-------------|
| `setupRTC()` | STUBBED | Returns `false`; records `ERR_RTC_READ` |
| `loopRTC()` | STUBBED | Empty body; called every `INTERVAL_RTC` (1 s) |
| `DateTime` struct | IMPLEMENTED | Packed struct: year, month, day, hour, minute, second, valid flag |
| `currentTime` | IMPLEMENTED | Global instance for data sharing |
| `ADDR_DS3231` | IMPLEMENTED | 0x68 (config.h) |

## Data Structure (from abr_firmware.ino)
```cpp
struct __attribute__((packed)) DateTime {
  uint16_t year = 0;
  uint8_t  month = 0, day = 0, hour = 0, minute = 0, second = 0;
  bool     valid = false;
} currentTime;
```

## Integration Points
- Called from main `loop()` at `INTERVAL_RTC` (1 s)
- Provides `currentTime` to `storage` (CSV timestamps), `display` (clock), `comm` (future)
- Shares I²C bus with `sensors` module — sequential access in main loop

## Files (Planned)
- `time.h` / `time.cpp` — Public API: `time_init()`, `time_read()`, `time_set()`, `time_is_valid()`, `time_to_iso8601(buffer, size)`
- `ds3231.h` — Thin wrapper for alarm, temperature, aging register if needed

## Dependencies
- `Wire.h`
- `RTClib` (provides `DateTime`, `RTC_DS3231`)

## Notes
- **Coin cell:** DS3231 module typically includes CR2032 holder. Verify battery present for backup.
- **I²C bus sharing:** DS3231 at 0x68, AHT20 at 0x38, BMP280 at 0x76/0x77 — no address conflict (compile-time check in `config.h`).
- **Timezone:** Store UTC in RTC. Apply offset only for display (configurable later).
- **Compile-time sync:** On first boot (RTC oscillator stopped flag), set from `__DATE__` `__TIME__`. This is approximate (compile → upload delay).
- **Accuracy:** DS3231 is ±2 ppm (≈1 minute/year). Good enough for logging; no need for GPS/NTP in Phase 2.
- **Alarm feature:** DS3231 has two alarms. Can wake MCU from power-down sleep via INT/SQW pin (connect to D2/D3 for external interrupt).
- **Temperature sensor:** DS3231 internal temp (±3°C) readable via `RTClib` — useful for diagnostic, not for weather.
- **Voltage warning:** DS3231 module voltage tolerance varies. Many modules are 5 V tolerant due to onboard regulator, but **verify your specific module's datasheet**. If not 5 V tolerant, the SmartElex level converter is REQUIRED on I²C lines.