# Power Management Module

## Purpose
Manages power consumption, supply monitoring, and low-power operation for the ABR weather station.

## Hardware Context
- **MCU:** Arduino Uno R3 (ATmega328P, 5 V logic, 16 MHz)
- **Supply:** USB (5 V) or external 7–12 V via barrel jack / Vin
- **Regulators:** Uno onboard 5 V (≈500 mA) and 3.3 V (≈150 mA) — limited current
- **Sensors/Modules:** All 3.3 V logic via SmartElex bi-directional level converter
- **Display:** ST7789 TFT (backlight is main consumer)
- **Storage:** MicroSD card (spikes during write)

## Planned Responsibilities
- **Voltage monitoring** — Read VCC (bandgap reference) and/or external supply via divider
- **Sleep modes** — Idle, ADC noise reduction, power-down between measurement intervals
- **Peripheral power control** — MOSFET/gate control for sensors, display, SD (if hardware supports)
- **Brown-out handling** — Graceful shutdown, data flush on low voltage
- **Watchdog** — Reset on firmware hang

## Current Status
**Planned** — No implementation yet. Not referenced in `abr_firmware.ino` stubs.

## Integration Points
- Called from main `loop()` to enter sleep between intervals
- `setupPower()` in `setup()` for initialisation
- Coordinates with `time` (RTC alarm wake), `sensors` (power-up settling), `display` (sleep/wake), `storage` (flush before sleep)

## Files (Planned)
- `power.h` / `power.cpp` — Public API: `power_init()`, `power_sleep(interval_ms)`, `power_get_vcc()`, `power_is_low()`

## Dependencies
- `avr/sleep.h`, `avr/power.h`, `avr/wdt.h`
- `LowPower` library (optional, lightweight alternative to manual register manipulation)

## Notes
- Uno R3 has limited low-power options vs. modern MCUs; consider deep sleep only if battery-powered
- 3.3 V regulator on Uno is weak (150 mA) — TFT + SD + sensors may exceed; measure actual draw
- Level converter is always-on; factor its quiescent current
- If solar/battery added later, this module expands to MPPT, charge control, SoC estimation
- Keep it simple for Phase 2; add sophistication only when deployment demands it