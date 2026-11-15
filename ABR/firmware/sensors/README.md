# Sensors Module

## Purpose
Reads and manages all environmental sensors: temperature, humidity, pressure (AHT20 + BMP280), and ambient light (TEMT6000).

## Hardware
| Sensor | Measurement | Interface | Address / Pin | Library (Planned) |
|--------|-------------|-----------|---------------|-------------------|
| AHT20 | Temperature (°C), Humidity (%RH) | I²C | 0x38 | `Adafruit_AHTX0` |
| BMP280 | Pressure (hPa), Temperature (°C) | I²C | 0x76 (SDO low) / 0x77 | `Adafruit_BMP280` |
| TEMT6000 | Illuminance (lux, relative) | Analog | A0 | Direct `analogRead()` + calibration |

## Pin Assignments (from config.h)
- I²C SDA: A4, SCL: A5 (shared with DS3231 RTC)
- Light sensor OUT: A0

## Planned Responsibilities
- **Initialisation** — Detect and configure each sensor, verify WHO_AM_I / chip IDs
- **Measurement scheduling** — Respect minimum intervals (AHT20: ~2 s, BMP280: ~1 ms, TEMT6000: instant)
- **Data acquisition** — Read raw values, convert to SI units, validate ranges
- **Error handling** — Mark readings invalid on I²C timeout, CRC error, out-of-range
- **Calibration** — TEMT6000 lux conversion (requires reference lux meter); BMP280 altitude compensation
- **Health monitoring** — Track consecutive failures, trigger re-initialisation

## Current Status
**Planned** — Stubs in `abr_firmware.ino`:
- `setupSensors()` — returns `false`
- `loopEnvironment()` — reads AHT20 + BMP280 → `currentEnv` (every `INTERVAL_ENV` = 5 s)
- `loopLight()` — reads TEMT6000 → `currentLight` (every `INTERVAL_LIGHT` = 2 s)

## Data Structures (from abr_firmware.ino)
```cpp
struct EnvReading {
  float temperature = NAN;  // °C
  float humidity    = NAN;  // %RH
  float pressure    = NAN;  // hPa
  bool  valid       = false;
} currentEnv;

struct LightReading {
  float lux         = NAN;  // lux (calibrated)
  bool  valid       = false;
} currentLight;
```

## Integration Points
- Called from main `loop()` at configured intervals
- Provides data to `storage` (CSV logging), `display` (local readout), `comm` (future remote)
- Shares I²C bus with `time` module (DS3231) — bus arbitration via sequential calls

## Files (Planned)
- `sensors.h` / `sensors.cpp` — Public API: `sensors_init()`, `sensors_read_env()`, `sensors_read_light()`
- Per-sensor drivers or thin wrappers: `aht20.h`, `bmp280.h`, `temt6000.h`
- Calibration constants in `config.h` or separate `calibration.h`

## Dependencies
- `Wire.h` (I²C)
- `Adafruit_AHTX0`, `Adafruit_BMP280`, `Adafruit_Sensor` (or lighter alternatives)

## Notes
- AHT20 and BMP280 share I²C bus — read sequentially, not simultaneously
- BMP280 temperature is secondary; prefer AHT20 for temperature (better accuracy)
- TEMT6000 is a phototransistor, not a calibrated lux meter — output is relative; document limitation
- I²C pull-ups: verify modules have them; Uno internal pull-ups (via `Wire`) are weak (20–50 kΩ)
- Handle `NAN` propagation — downstream modules must check `.valid` before using values
- Measurement intervals in `config.h` are minimums; actual loop timing depends on `millis()` scheduling