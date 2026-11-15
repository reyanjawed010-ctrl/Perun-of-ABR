# AGENTS.md — PI Coding Agent Instructions for ABR (Perun Project)

## Project Identity

- **Project**: Perun (broader software + open-source hardware ecosystem)
- **Hardware**: ABR (ابر) — low-cost, modular weather monitoring station
- **Primary platform**: Arduino Uno R3
- **Development approach**: Open-source, modular, community-driven
- **License**: MIT
- **Repository**: `praveenshabnam010-ctrl/Perun-of-ABR` (public, `main` branch)
- **Clone path**: `/home/samimj/Projects/Perun-of-ABR`

## Project Status

The physical weather station has **not yet been built**. No components have been purchased, acquired, or tested. All hardware documentation is **proposed / unverified**. No firmware has been written.

Current status: **Planning and documentation phase**. Roadmap Phase 0 and Phase 1 are in development; Phase 2 (firmware skeleton) is planned but blocked until physical hardware is available.

---

## Core Objectives (from ABR guide)

All development decisions must follow these principles:

1. **Affordability** — Keep cost low. Prefer commonly available components.
2. **Modularity** — Independent, replaceable modules.
3. **Open source** — Code, docs, and design files publicly accessible.
4. **Reproducibility** — Anyone can replicate using docs and common parts.
5. **Reliability** — Dependable measurements over unnecessary features.
6. **Simplicity** — Avoid overengineering.

## Confirmed Hardware Components (BOM — ₹830 total)

| Component | Qty | Price | Interface | Notes |
|-----------|-----|-------|-----------|-------|
| DS3231 RTC Module | 1 | ₹219 | I²C | Real-time clock |
| MicroSD Card Reader Module | 1 | ₹39 | SPI | Local data storage |
| CJMCU TEMT6000 Ambient Light Sensor | 1 | ₹89 | Analog (A0) | Light intensity |
| AHT20 + BMP280 Module | 1 | ₹149 | I²C | Temp, humidity, pressure |
| GoldenMorning 1.9" ST7789 TFT | 1 | ₹289 | SPI | Local display |
| SmartElex Bi-Directional Logic Level Converter | 1 | ₹45 | — | 5 V ↔ 3.3 V |
| **Total** | | **₹830** | | |

**Not included:** Rain gauge (removed by user decision).

**Already available:** Arduino Uno R3, microSD card, breadboard, jumper wires.

**Prices from:** Robu.in (12 November 2026) — may vary by region/supplier.

## Pin Map (Arduino Uno R3 — PROPOSED, NOT VERIFIED)

| Bus | Signal | Pin | Device | Notes |
|-----|--------|-----|--------|-------|
| I²C | SDA | A4 | DS3231, AHT20+BMP280 | Shared bus |
| I²C | SCL | A5 | DS3231, AHT20+BMP280 | Shared bus |
| SPI | MOSI | D11 | ST7789 TFT, MicroSD | Shared bus |
| SPI | MISO | D12 | ST7789 TFT, MicroSD | Shared bus |
| SPI | SCK | D13 | ST7789 TFT, MicroSD | Shared bus |
| SPI | CS | D10 | ST7789 TFT | Active low |
| SPI | CS | D4 | MicroSD | Active low |
| SPI | DC | D7 | ST7789 TFT | Data/command |
| SPI | RST | D8 | ST7789 TFT | Reset |
| Analog | OUT | A0 | CJMCU TEMT6000 | Light sensor |
| Power | 5 V | — | DS3231 | Uno regulated output |
| Power | 3.3 V | — | AHT20+BMP280, TFT, MicroSD | Limited current capacity |

**Critical:** Arduino Uno R3 is 5 V logic. Many modules are 3.3 V. Use the SmartElex level converter where required. Verify every pin assignment against datasheets before wiring. No hardware testing done yet.

## Repository Structure (current)

```
Perun-of-ABR/
├── README.md
├── LICENSE (MIT)
├── CONTRIBUTING.md
├── .gitignore
└── ABR/
    ├── documentation/
    │   ├── index.md
    │   ├── hardware-overview.md
    │   ├── wiring.md
    │   └── roadmap.md
    └── hardware/
        ├── BOM.md
        └── pinmap.md
```

**No `ABR/firmware/` directory exists yet.** Firmware development (Phase 2) is blocked until physical hardware is available for testing.

---

## Rules for Working on This Project (from ABR guide)

### Things you MUST NOT do
- Do not invent hardware specifications.
- Do not pretend the physical station has already been built.
- Do not fabricate testing results, measurements, or accuracy claims.
- Do not add expensive components without justification.
- Do not introduce unnecessary IoT infrastructure or cloud dependencies.
- Do not overcomplicate the software architecture.
- Do not create large amounts of placeholder content.
- Do not add dependencies without explaining their purpose.
- Do not make unrelated changes to the repository.
- Do not commit API keys, credentials, or other secrets.

### Documentation standards
- Use **British English**.
- Prefer clear, straightforward language.
- Explain technical terms when appropriate.
- Use status labels: **Planned** / **In development** / **Implemented** / **Tested**.
- Do not document proposed features as implemented.
- Never invent test results or specifications.

### Coding standards (Arduino C/C++)
- Write readable, maintainable code.
- Use meaningful variable and function names.
- Keep modules responsible for specific tasks.
- Avoid unnecessary libraries; prefer lightweight dependencies.
- Comment important or non-obvious logic.
- Handle sensor errors and missing readings gracefully.
- Avoid blocking operations that interfere with measurement scheduling.
- Respect sensor measurement intervals.
- Keep units consistent.

### Hardware rules
- Prefer standard protocols (I²C, SPI).
- Document all pin assignments.
- Verify compatibility against datasheets before recommending components.
- Consider price, availability, reliability, and power consumption.

---

## Development Workflow

1. Inspect the existing repository before making changes.
2. Read existing documentation and source code.
3. Understand the intended functionality.
4. Identify missing information.
5. Propose an implementation before making major architectural changes.
6. Implement changes in manageable steps.
7. Check code for errors and compatibility.
8. Update relevant documentation.
9. Summarise the changes made.

Do not overwrite user-created work without permission. If a technical decision affects cost, hardware compatibility, or architecture, explain trade-offs first.

---

## What Has Been Done (by the coding agent)

- Cloned `praveenshabnam010-ctrl/Perun-of-ABR` to `/home/samimj/Projects/Perun-of-ABR`
- Added `.gitignore` (Arduino build artefacts, IDE files, secrets)
- Added `CONTRIBUTING.md` with contribution guidelines, coding standards, AI assistance note
- Switched `LICENSE` from GPL-3.0 to MIT
- Updated `README.md` footer attribution (OpenCode + NVIDIA Nemotron 3 Ultra)
- Created `ABR/documentation/index.md`
- Created `ABR/documentation/hardware-overview.md` (updated for confirmed components)
- Created `ABR/documentation/wiring.md` (updated for confirmed components)
- Created `ABR/documentation/roadmap.md`
- Created `ABR/hardware/BOM.md` (₹830 total, confirmed components)
- Created `ABR/hardware/pinmap.md` (proposed pin assignments for confirmed components)

## What Remains

- **Phase 2: Firmware skeleton** — `abr_firmware.ino`, `config.h`, sensor modules. Blocked until hardware arrives.
- **Phase 3: Integration** — RTC timestamps, CSV logging, TFT display output.
- **Phase 4: Reliability** — Error handling, measurement intervals, calibration.
- **Phase 5: Prototype** — Physical build, test results, photos, verified wiring diagram.
- **Phase 6: Release** — Tagged version, setup/troubleshooting docs.

## AI Assistance Note

This project uses AI tools (OpenCode, NVIDIA Nemotron 3 Ultra) for coding assistance. All design decisions, review, and responsibility remain with the human maintainers. AI-generated content must be verified before committing. AI is a tool, not the main character.

---

## Key Files Reference

| File | Purpose |
|------|---------|
| `README.md` | Project overview, status, principles |
| `CONTRIBUTING.md` | Contribution guidelines, coding standards |
| `ABR/documentation/hardware-overview.md` | Architecture, components, power/logic levels |
| `ABR/documentation/wiring.md` | Bus overview, connection notes, pre-wiring checklist |
| `ABR/hardware/pinmap.md` | Proposed Arduino pin assignments |
| `ABR/hardware/BOM.md` | Component list with prices |
| `ABR/documentation/roadmap.md` | Development phases and status |
| `ABR/documentation/index.md` | Documentation index |
