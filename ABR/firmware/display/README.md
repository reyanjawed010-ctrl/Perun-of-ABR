# Display Module

## Purpose
Drives the 1.9" ST7789 TFT display (170×320, SPI) for local readout of sensor data and station status.

## Hardware
- **Module:** GoldenMorning 1.9" ST7789 TFT
- **Interface:** SPI (shared bus with MicroSD)
- **Pins (from config.h):**
  - CS: D10 (active low)
  - DC: D7 (data/command select)
  - RST: D8 (reset)
  - MOSI/SCK: Shared SPI pins D11/D13
- **Power:** 3.3 V logic via level converter
- **Rotation:** Landscape (TFT_ROTATION = 1)

## Planned Responsibilities
- **Initialisation** — Reset sequence, driver init, rotation, backlight control
- **Splash screen** — Show station name, version, and boot status
- **Data screens** — Cycle through: current readings, min/max, status, settings
- **Status indicators** — SD card, RTC, sensor health, battery (if added)
- **Sleep/wake** — Display power management for energy saving

## Current Status
**Planned** — Stub in `abr_firmware.ino`:
- `setupTFT()` — returns `false` (hardware not present)
- `loopDisplay()` — empty, called every `INTERVAL_DISPLAY` (10 s)

## Integration Points
- Reads `currentEnv`, `currentLight`, `currentTime` from main firmware
- Requires `sdCardReady`, `rtcReady`, `sensorsReady` flags for status display
- Shares SPI bus with `storage` module (MicroSD) — CS pin management critical

## Files (Planned)
- `display.h` / `display.cpp` — Public API: `display_init()`, `display_update()`, `display_sleep()`, `display_wake()`
- Font definitions, bitmap assets, screen layout helpers

## Dependencies
- `Adafruit_ST7789` + `Adafruit_GFX` (or lightweight alternative)
- `SPI.h`

## Notes
- Verify ST7789 variant (some use different init sequences)
- Backlight control via PWM if hardware supports it
- Handle SPI bus contention with MicroSD — always deassert CS when not active
- Display updates must be non-blocking; use state machine if drawing is slow