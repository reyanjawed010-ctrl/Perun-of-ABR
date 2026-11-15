# Storage Module

## Purpose
Manages MicroSD card for persistent CSV logging of sensor readings with timestamps.

## Hardware
- **Module:** Generic MicroSD card reader (SPI)
- **Interface:** SPI (shared with ST7789 TFT)
- **Pins (from config.h):**
  - CS: D4 (active low)
  - MOSI/SCK: Shared SPI pins D11/D13
- **Card:** FAT32 formatted, ≤32 GB (SDHC)
- **Power:** 3.3 V via level converter

## Planned Responsibilities
- **Card detection & init** — Verify presence, type, filesystem
- **Log file management** — Create `abr_log.csv` with header, append rows
- **Buffered writes** — Accumulate readings in RAM (`LOG_BUFFER_SIZE` = 512 bytes), flush periodically
- **Error handling** — Card full, write error, card removed — log to serial, retry, mark `sdCardReady = false`
- **Header format** — `timestamp,temp_c,humidity_rh,pressure_hpa,lux`

## Current Status
**Planned** — Stubs in `abr_firmware.ino`:
- `setupSD()` — returns `false`
- `loopLog()` — empty, called every `INTERVAL_LOG` (60 s)

## CSV Format (Planned)
```
timestamp,temp_c,humidity_rh,pressure_hpa,lux
2025-01-15T10:30:00Z,23.4,55.2,1013.2,1250
2025-01-15T10:31:00Z,23.5,55.1,1013.1,1240
```
- Timestamp from DS3231 (UTC, ISO 8601)
- Missing/invalid readings → empty field

## Integration Points
- Called from main `loop()` at `INTERVAL_LOG`
- Consumes `currentEnv`, `currentLight`, `currentTime` (checks `.valid` flags)
- Shares SPI bus with `display` module — CS pin management critical
- Requires `sdCardReady` flag from `setupSD()`

## Files (Planned)
- `storage.h` / `storage.cpp` — Public API: `storage_init()`, `storage_log(env, light, time)`, `storage_flush()`, `storage_is_ready()`
- `csv.h` — Lightweight CSV formatting helpers

## Dependencies
- `SPI.h`
- `SD.h` (Arduino SD library) or `SdFat` (more robust, larger)

## Notes
- **SPI bus contention:** Always `digitalWrite(PIN_SD_CS, HIGH)` before using TFT, and vice versa. Consider a `spi_transaction_begin/end` helper.
- **Power loss safety:** Call `storage_flush()` before sleep or on low-voltage warning.
- **Card wear:** Minimise writes — batch at `INTERVAL_LOG` (60 s default). Avoid `sync()` on every row.
- **Filename strategy:** Single file `abr_log.csv` for simplicity. Rotate daily/monthly only if file size becomes an issue.
- **Header:** Write once on first boot; detect existing file to avoid duplicate headers.
- **Timestamp source:** DS3231 RTC via `time` module. If RTC not ready, use `millis()` offset from boot (mark as relative).