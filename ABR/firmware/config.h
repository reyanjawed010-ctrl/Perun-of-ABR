/**
 * ABR Weather Station — Configuration
 * Pin assignments and measurement intervals.
 * Update if hardware changes.
 */

#ifndef CONFIG_H
#define CONFIG_H

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

// ── I²C addresses ─────────────────────────────────────────────────────────
#define ADDR_DS3231       0x68
#define ADDR_AHT20        0x38
#define ADDR_BMP280       0x76  // or 0x77 depending on SDO pin

// ── Measurement intervals (milliseconds) ──────────────────────────────────
#define INTERVAL_RTC      1000UL      // RTC read
#define INTERVAL_ENV      5000UL      // Temp/humidity/pressure
#define INTERVAL_LIGHT    2000UL      // Light sensor
#define INTERVAL_LOG      60000UL     // CSV log to SD
#define INTERVAL_DISPLAY  10000UL     // TFT update

// ── Display settings ──────────────────────────────────────────────────────
#define TFT_WIDTH         170
#define TFT_HEIGHT        320
#define TFT_ROTATION      1

// ── SD card ───────────────────────────────────────────────────────────────
#define LOG_FILENAME      "abr_log.csv"
#define LOG_BUFFER_SIZE   512

// ── Debug ─────────────────────────────────────────────────────────────────
#define SERIAL_BAUD       115200
#define DEBUG_ENABLED     1

#endif // CONFIG_H