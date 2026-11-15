# ABR Documentation

ABR (ابر) is the weather station hardware project within Perun. It is intended to be an affordable, modular and reproducible station for monitoring local environmental conditions.

## Current status

**Phase 0 & 1: In development** — Repository foundations and planning documentation are drafted but **not validated on physical hardware**.
**Phase 2+: Planned** — Firmware, integration, prototype and release are blocked until physical hardware is acquired and tested.

The physical station has not been built or tested. All hardware is proposed and has not yet been acquired.

## Documents

| Document | Status | Description |
|----------|--------|-------------|
| [Hardware overview](hardware-overview.md) | In development | Architecture, components, power/logic levels, open questions |
| [Wiring](wiring.md) | In development | Bus overview, connection notes, pre-wiring checklist |
| [Pin map](../hardware/pinmap.md) | In development | Proposed Arduino pin assignments, I²C addresses, voltage compatibility |
| [Bill of materials](../hardware/BOM.md) | In development | 6 confirmed components, ₹830 total, verification checklist |
| [Roadmap](roadmap.md) | In development | 6-phase development plan with status labels |

## Quick links

- [Main README](../../README.md)
- [Contributing guidelines](../../CONTRIBUTING.md)
- [Licence](../../LICENSE)

## Current limitations

- No physical hardware is available yet.
- No firmware has been implemented.
- No measurements or test results exist.
- Component variants, logic levels and compatibility are unverified.
- Pin assignments are proposals and must be verified against datasheets before wiring.
