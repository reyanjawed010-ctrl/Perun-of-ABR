# Utilities Module

## Purpose
Common helper functions, macros, and data structures used across firmware modules.

## Planned Responsibilities
- **String formatting** — Safe `snprintf` wrappers, float-to-string with fixed precision
- **Unit conversions** — hPa ↔ mmHg, °C ↔ °F, lux ↔ W/m² (if calibrated)
- **Data validation** — Range checks for sensor readings, `isnan()` helpers
- **Circular buffer** — Lightweight ring buffer for `storage` module write batching
- **CRC/checksum** — Data integrity for logs or future comm protocol
- **Timing helpers** — Non-blocking interval checks, `millis()` rollover-safe arithmetic
- **Debug/assert** — Conditional compilation macros for `DEBUG_ENABLED`

## Current Status
**PARTIALLY IMPLEMENTED** — Several helpers exist inline in `abr_firmware.ino`; no separate `utils.h`/`utils.cpp` module yet.

## Implementation Status
| Function / Feature | Status | Location |
|--------------------|--------|----------|
| `logToSerial()` | IMPLEMENTED | `abr_firmware.ino` — variadic, PROGMEM format support |
| `recordError()` | IMPLEMENTED | `abr_firmware.ino` — error tracking with timestamp/context |
| `freeRam()` | IMPLEMENTED | `abr_firmware.ino` — AVR heap/stack diagnostic |
| `i2cReadWithRetry()` | PLANNED | Commented template in `abr_firmware.ino` |
| `spiTransferTFT()` / `spiTransferSD()` | PLANNED | Commented templates in `abr_firmware.ino` |
| `interval_elapsed()` | PLANNED | — |
| `fmt_float()` / `fmt_iso8601()` | PLANNED | — |
| Validation helpers (`valid_temp`, etc.) | PLANNED | — |
| `RingBuffer` template | PLANNED | — |
| CRC/checksum | PLANNED | — |

## Integration Points
- Header-only or single `.cpp` included by all modules
- No external dependencies beyond Arduino core
- Must not introduce circular dependencies

## Files (Planned)
- `utils.h` — Public API: macros, inline functions, type definitions
- `utils.cpp` — Non-inline implementations (buffer, CRC, etc.)

## Example API (Planned)
```cpp
// Timing
bool interval_elapsed(unsigned long* last, unsigned long interval);

// Strings
void fmt_float(char* buf, size_t len, float val, uint8_t decimals);
void fmt_iso8601(char* buf, size_t len, const DateTime& dt);

// Validation
bool valid_temp(float c);      // -40..+85
bool valid_humidity(float rh); // 0..100
bool valid_pressure(float hpa); // 300..1100
bool valid_lux(float lux);     // 0..200000

// Buffer
template<typename T, size_t N>
class RingBuffer { ... };
```

## Notes
- Keep it minimal — only add what multiple modules actually need
- Avoid dynamic allocation (`malloc`/`new`) — use static buffers
- Prefer `constexpr`/`inline` for header-only helpers
- `DEBUG_ENABLED` from `config.h` gates all debug output
- Document assumptions (e.g., float precision, endianness) for portability
- Extract existing `logToSerial()`, `recordError()`, `freeRam()` from `abr_firmware.ino` into this module when created