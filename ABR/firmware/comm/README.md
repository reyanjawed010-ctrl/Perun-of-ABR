# Communication Module

## Purpose
Handles all external communication for the ABR weather station. Currently reserved for future use.

## Planned Responsibilities
- **Serial/USB** — Primary debug and configuration interface (implemented in `abr_firmware.ino` as `logToSerial()`)
- **Future wireless** — LoRa, Wi-Fi, or BLE modules for remote data retrieval
- **Protocol handling** — Structured message encoding/decoding (JSON, CBOR, or custom binary)
- **Command parsing** — Remote configuration and control commands

## Current Status
**PLANNED** — No communication module implementation exists. The main firmware uses `Serial` directly for debug output via `logToSerial()` (IMPLEMENTED in `abr_firmware.ino`).

## Implementation Status
| Function / Feature | Status | Location |
|--------------------|--------|----------|
| `logToSerial()` (debug output) | IMPLEMENTED | `abr_firmware.ino` |
| `comm_init()` | PLANNED | — |
| `comm_poll()` | PLANNED | — |
| `comm_send()` | PLANNED | — |
| Wireless protocol support | PLANNED | — |

## Integration Points
- Called from main loop if/when remote communication is added
- May share SPI/I²C buses with other modules
- Configuration via `config.h` (pins, baud rates, protocol settings)

## Files (Planned)
- `comm.h` / `comm.cpp` — Public API: `comm_init()`, `comm_poll()`, `comm_send()`
- Protocol-specific implementations as needed

## Notes
- Keep communication optional; station must operate standalone with SD logging
- Prefer low-power, long-range protocols for remote deployments
- Avoid cloud dependencies; local-first architecture
- `DEBUG_ENABLED` in `config.h` gates all debug serial output