# Display Module

## Purpose
Drives the 1.9" ST7789 TFT display (170×320, SPI) for local readout of sensor data and station status.

## Hardware
- **Module:** GoldenMorning 1.9" ST7789 TFT
- **Interface:** SPI (shared bus with MicroSD)
- **Pins (from config.h, verified against pinmap.md):**
  - CS: D10 (active low)
  - DC: D7 (data/command select)
  - RST: D8 (reset)
  - MOSI: D11, MISO: D12, SCK: D13 (shared SPI bus)
- **Power:** 3.3 V logic via SmartElex bi-directional level converter (required — ST7789 is not 5 V tolerant)
- **Rotation:** Landscape (TFT_ROTATION = 1)

## Planned Responsibilities
- **Initialisation** — Reset sequence, driver init, rotation, backlight control
- **Splash screen** — Show station name, version, and boot status
- **Data screens** — Cycle through: current readings, min/max, status, settings
- **Status indicators** — SD card, RTC, sensor health, battery (if added)
- **Sleep/wake** — Display power management for energy saving

## Current Status
**STUBBED** — Functions exist in `abr_firmware.ino` but return false / do nothing:

## Implementation Status
| Function | Status | Description |
|----------|--------|-------------|
| `setupTFT()` | STUBBED | Returns `false`; records `ERR_TFT_INIT` |
| `loopDisplay()` | STUBBED | Empty body; called every `INTERVAL_DISPLAY` (10 s) |
| `spiBeginTFT()` / `spiEndTFT()` | IMPLEMENTED | SPI transaction guards with CS management |
| `SPI_SETTINGS_TFT` | IMPLEMENTED | PROGMEM SPI settings (8 MHz, MODE0) |

## Integration Points
- Reads `currentEnv`, `currentLight`, `currentTime` from main firmware
- Requires `sdCardReady`, `rtcReady`, `sensorsReady` flags for status display
- Shares SPI bus with `storage` module (MicroSD) — CS pin management via `spiBeginTFT()`/`spiEndTFT()`

## Files (Planned)
- `display.h` / `display.cpp` — Public API: `display_init()`, `display_update()`, `display_sleep()`, `display_wake()`
- Font definitions, bitmap assets, screen layout helpers

## Dependencies
- `Adafruit_ST7789` + `Adafruit_GFX` (or lightweight alternative)
- `SPI.h`

## Notes
- Verify ST7789 variant (some use different init sequences)
- Backlight control via PWM if hardware supports it
- Handle SPI bus contention with MicroSD — always deassert CS when not active (use `spiBeginTFT()`/`spiEndTFT()`)
- Display updates must be non-blocking; use state machine if drawing is slow
- **Voltage warning:** ST7789 operates at 3.3 V logic. The SmartElex level converter is REQUIRED. Do not connect directly to Uno 5 V pins.