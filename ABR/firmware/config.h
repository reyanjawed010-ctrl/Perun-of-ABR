/**
 * ABR Weather Station — Configuration
 * Pin assignments and measurement intervals.
 * Update if hardware changes.
 *
 * ⚠ VOLTAGE / LOGIC LEVEL WARNINGS ⚠
 * ------------------------------------------------------------
 * - Arduino Uno R3 operates at 5 V logic.
 * - ST7789 TFT, MicroSD card reader, AHT20+BMP280 module are 3.3 V logic
 *   and are NOT 5 V tolerant. Damage will occur if driven directly at 5 V.
 * - SmartElex bi-directional logic level converter is REQUIRED for these
 *   modules. Verify wiring against pinmap.md before powering on.
 * - DS3231 RTC module: may be 5 V tolerant (verify your specific module's
 *   datasheet). If not 5 V tolerant, it also requires level shifting.
 * - CJMCU TEMT6000 light sensor: logic level varies by module; check yours.
 * - Uno 3.3 V rail is limited to ~150 mA total. Do not exceed this budget.
 * ------------------------------------------------------------
 */

#ifndef CONFIG_H
#define CONFIG_H

// ── Compile-time architecture check ───────────────────────────────────────
#ifndef ARDUINO_ARCH_AVR
#error "ABR firmware targets AVR architecture (Arduino Uno R3). Unsupported board."
#endif

// ── Pin assignments (Arduino Uno R3) ──────────────────────────────────────
// I²C bus (shared)
#define PIN_I2C_SDA       A4
#define PIN_I2C_SCL       A5

// SPI bus (shared)
#define PIN_SPI_MOSI      11
#define PIN_SPI_MISO      12
#define PIN_SPI_SCK       13

// ST7789 TFT (1.9", 170×320)
#define PIN_TFT_CS        10
#define PIN_TFT_DC        7
#define PIN_TFT_RST       8

// MicroSD card reader
#define PIN_SD_CS         4

// Light sensor (analog)
#define PIN_LIGHT_OUT     A0

// ── Compile-time pin conflict checks ──────────────────────────────────────
#if PIN_TFT_CS == PIN_SD_CS
#error "Pin conflict: PIN_TFT_CS and PIN_SD_CS cannot be the same pin."
#endif

#if PIN_TFT_DC == PIN_TFT_CS || PIN_TFT_DC == PIN_SD_CS || PIN_TFT_DC == PIN_TFT_RST
#error "Pin conflict: PIN_TFT_DC conflicts with another TFT/SD pin."
#endif

#if PIN_TFT_RST == PIN_TFT_CS || PIN_TFT_RST == PIN_SD_CS
#error "Pin conflict: PIN_TFT_RST conflicts with another TFT/SD pin."
#endif

#if PIN_LIGHT_OUT == PIN_I2C_SDA || PIN_LIGHT_OUT == PIN_I2C_SCL
#error "Pin conflict: PIN_LIGHT_OUT conflicts with I²C pins."
#endif

// ── I²C addresses ─────────────────────────────────────────────────────────
#define ADDR_DS3231       0x68
#define ADDR_AHT20        0x38
#define ADDR_BMP280       0x76  // or 0x77 depending on SDO pin

// Compile-time I²C address conflict check
#if ADDR_DS3231 == ADDR_AHT20 || ADDR_DS3231 == ADDR_BMP280 || ADDR_AHT20 == ADDR_BMP280
#error "I²C address conflict: two devices share the same address."
#endif

// ── Measurement intervals (milliseconds) ──────────────────────────────────
#define INTERVAL_RTC      1000UL      // RTC read
#define INTERVAL_ENV      5000UL      // Temp/humidity/pressure
#define INTERVAL_LIGHT    2000UL      // Light sensor
#define INTERVAL_LOG      60000UL     // CSV log to SD
#define INTERVAL_DISPLAY  10000UL     // TFT update
#define INTERVAL_WATCHDOG 1000UL      // Watchdog reset

// ── Display settings ──────────────────────────────────────────────────────
#define TFT_WIDTH         170
#define TFT_HEIGHT        320
#define TFT_ROTATION      1

// ── SD card ───────────────────────────────────────────────────────────────
#define LOG_FILENAME      "abr_log.csv"
#define LOG_BUFFER_SIZE   128

// Compile-time buffer size warning
#if LOG_BUFFER_SIZE > 256
#warning "LOG_BUFFER_SIZE exceeds 256 bytes; may strain Uno's 2 KB RAM."
#endif

// ── Debug ─────────────────────────────────────────────────────────────────
#define SERIAL_BAUD       115200
#define DEBUG_ENABLED     1

// ── Watchdog ──────────────────────────────────────────────────────────────
// AVR WDTO_8S = ~8 seconds. Loop must call wdt_reset() more frequently.
// Main loop calls loopWatchdog() every INTERVAL_WATCHDOG.

// Compile-time check: WDT timeout must exceed max loop interval
#if INTERVAL_WATCHDOG > 8000
#error "INTERVAL_WATCHDOG exceeds WDTO_8S (~8s); watchdog will reset"
#endif

// ── SPI Safety ────────────────────────────────────────────────────────────
// Ensure CS pins are different (already checked above) and are valid
#if PIN_TFT_CS > 19 || PIN_SD_CS > 19
#error "CS pins must be valid Arduino Uno pins (0-19/A0-A5)"
#endif

// SPI clock divider sanity: Uno max SPI is F_CPU/2 = 8 MHz
// TFT setting 8 MHz, SD setting 4 MHz - both within limits

#endif // CONFIG_H