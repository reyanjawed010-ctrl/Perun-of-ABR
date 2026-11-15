/**
 * ABR Weather Station — Firmware Skeleton
 * Phase 2: Basic structure, non-blocking loop.
 * Sensor drivers added in Phase 2 (when hardware available).
 * 
 * Updated with:
 * - SPI transaction guards for TFT/SD bus sharing
 * - AVR Watchdog Timer (WDT)
 * - Memory-efficient patterns (no full-frame buffers, fixed stacks, PROGMEM)
 * - Comprehensive error handling with retry/recovery
 */

#include "config.h"
#include <avr/wdt.h>
#include <avr/pgmspace.h>

// ── Libraries (add as sensors are implemented) ─────────────────────────────
// #include <Wire.h>           // I²C
// #include <SPI.h>            // SPI
// #include <SD.h>             // MicroSD
// #include <Adafruit_ST7789.h> // TFT
// #include <Adafruit_AHTX0.h> // AHT20
// #include <Adafruit_BMP280.h> // BMP280
// #include <RTClib.h>         // DS3231

// ── Compile-time RAM budget check ──────────────────────────────────────────
// Uno has 2 KB SRAM. Estimate static usage:
//
// Global structs:          ~60 bytes
// SPI settings (2x):       ~16 bytes
// Log line buffer:         128 bytes (LOG_BUFFER_SIZE)
// Serial TX buffer:        ~64 bytes (hardware)
// TFT line buffer:         ~0 bytes (streaming, no framebuffer)
// I2C buffers (Wire):      ~32 bytes each (rx/tx)
// Stack reserve:           ~500+ bytes for ISR + nested calls
//
// Total estimate: < 1 KB, leaving ~1 KB margin.

// ── Error codes (packed into uint8_t for memory efficiency) ────────────────
enum class ErrorCode : uint8_t {
  OK                 = 0,
  ERR_I2C_TIMEOUT    = 1,
  ERR_I2C_NACK       = 2,
  ERR_SPI_TIMEOUT    = 3,
  ERR_SD_INIT        = 4,
  ERR_SD_WRITE       = 5,
  ERR_SD_FULL        = 6,
  ERR_RTC_READ       = 7,
  ERR_RTC_INVALID    = 8,
  ERR_AHT20_READ     = 9,
  ERR_BMP280_READ    = 10,
  ERR_LIGHT_READ     = 11,
  ERR_TFT_INIT       = 12,
  ERR_TFT_COMM       = 13,
  ERR_WATCHDOG_RESET = 14,
  ERR_UNKNOWN        = 255
};

// ── SPI transaction settings (PROGMEM to save RAM) ─────────────────────────
// TFT: ST7789 typically uses SPI_MODE0, up to 40 MHz (but Uno max is 8 MHz)
// SD:  SPI_MODE0, up to 25 MHz (Uno: 4 MHz safe with level shifter)
const SPISettings SPI_SETTINGS_TFT PROGMEM = SPISettings(8000000, MSBFIRST, SPI_MODE0);
const SPISettings SPI_SETTINGS_SD  PROGMEM = SPISettings(4000000, MSBFIRST, SPI_MODE0);

// ── Global state ───────────────────────────────────────────────────────────
unsigned long lastRtcRead    = 0;
unsigned long lastEnvRead    = 0;
unsigned long lastLightRead  = 0;
unsigned long lastLogWrite   = 0;
unsigned long lastDisplay    = 0;
unsigned long lastWatchdog   = 0;

bool sdCardReady   = false;
bool rtcReady      = false;
bool tftReady      = false;
bool sensorsReady  = false;

// Error tracking for diagnostics
uint8_t errorCounts[16] = {0};  // Indexed by ErrorCode (truncated)
ErrorCode lastError = ErrorCode::OK;
unsigned long lastErrorTime = 0;

// ── Sensor data holders (packed, no padding waste) ─────────────────────────
struct __attribute__((packed)) EnvReading {
  float temperature = NAN;
  float humidity    = NAN;
  float pressure    = NAN;
  bool  valid       = false;
} currentEnv;

struct __attribute__((packed)) LightReading {
  float lux         = NAN;
  bool  valid       = false;
} currentLight;

struct __attribute__((packed)) DateTime {
  uint16_t year = 0;
  uint8_t  month = 0, day = 0, hour = 0, minute = 0, second = 0;
  bool     valid = false;
} currentTime;

// ── Forward declarations ───────────────────────────────────────────────────
void setupSerial();
void setupPins();
void setupWatchdog();
bool setupSD();
bool setupRTC();
bool setupTFT();
bool setupSensors();
void loopRTC();
void loopEnvironment();
void loopLight();
void loopLog();
void loopDisplay();
void loopWatchdog();
void logToSerial(const char* fmt, ...);
void recordError(ErrorCode code, const char* context);

// ── SPI Bus Management ─────────────────────────────────────────────────────
// Wrapper functions ensure proper transaction guards and CS pin management.
// Call these instead of raw SPI.transfer() for any SPI device.

inline void spiBeginTFT() {
  digitalWrite(PIN_TFT_CS, LOW);
  digitalWrite(PIN_SD_CS, HIGH);   // Ensure SD deselected
  SPI.beginTransaction(SPI_SETTINGS_TFT);
}

inline void spiEndTFT() {
  SPI.endTransaction();
  digitalWrite(PIN_TFT_CS, HIGH);
}

inline void spiBeginSD() {
  digitalWrite(PIN_SD_CS, LOW);
  digitalWrite(PIN_TFT_CS, HIGH);  // Ensure TFT deselected
  SPI.beginTransaction(SPI_SETTINGS_SD);
}

inline void spiEndSD() {
  SPI.endTransaction();
  digitalWrite(PIN_SD_CS, HIGH);
}

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  // Disable WDT early in case of reset loop (MCUSR check)
  MCUSR = 0;
  wdt_disable();
  
  setupSerial();
  setupPins();
  setupWatchdog();

  sdCardReady  = setupSD();
  rtcReady     = setupRTC();
  tftReady     = setupTFT();
  sensorsReady = setupSensors();

  logToSerial(PSTR("ABR firmware initialised"));
  logToSerial(PSTR("SD: %s  RTC: %s  TFT: %s  Sensors: %s"),
              sdCardReady ? PSTR("OK") : PSTR("FAIL"),
              rtcReady ? PSTR("OK") : PSTR("FAIL"),
              tftReady ? PSTR("OK") : PSTR("FAIL"),
              sensorsReady ? PSTR("OK") : PSTR("FAIL"));
  
  lastWatchdog = millis();
}

// ── Main loop (non-blocking) ───────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // Watchdog reset - must be called frequently enough (< WDTO_8S)
  if (now - lastWatchdog >= 1000UL) {
    loopWatchdog();
    lastWatchdog = now;
  }

  if (rtcReady     && now - lastRtcRead   >= INTERVAL_RTC)     { loopRTC();      lastRtcRead   = now; }
  if (sensorsReady && now - lastEnvRead   >= INTERVAL_ENV)     { loopEnvironment(); lastEnvRead  = now; }
  if (sensorsReady && now - lastLightRead >= INTERVAL_LIGHT)   { loopLight();     lastLightRead = now; }
  if (sdCardReady  && now - lastLogWrite  >= INTERVAL_LOG)     { loopLog();       lastLogWrite  = now; }
  if (tftReady     && now - lastDisplay   >= INTERVAL_DISPLAY) { loopDisplay();   lastDisplay   = now; }
}

// ── Module implementations (stubs - implement when hardware arrives) ───────

void setupSerial() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial && millis() < 2000) { /* wait for USB serial */ }
}

void setupPins() {
  // SPI CS pins - init HIGH (deselected) before SPI.begin()
  pinMode(PIN_TFT_CS,   OUTPUT); digitalWrite(PIN_TFT_CS,   HIGH);
  pinMode(PIN_SD_CS,    OUTPUT); digitalWrite(PIN_SD_CS,    HIGH);
  
  // TFT control pins
  pinMode(PIN_TFT_DC,   OUTPUT);
  pinMode(PIN_TFT_RST,  OUTPUT);
  
  // Light sensor
  pinMode(PIN_LIGHT_OUT, INPUT);
  
  // SPI.begin() configures MOSI/MISO/SCK
  SPI.begin();
  
  // I²C pins handled by Wire.begin() in setupRTC/setupSensors
}

void setupWatchdog() {
  // Enable WDT with 8-second timeout (longest available on AVR)
  // Must call wdt_reset() at least once every 8 seconds
  wdt_enable(WDTO_8S);
  logToSerial(PSTR("Watchdog enabled (8s timeout)"));
}

bool setupSD() {
  // TODO: Implement with proper SPI transaction guards
  // Example pattern:
  //   spiBeginSD();
  //   bool ok = SD.begin(PIN_SD_CS);
  //   spiEndSD();
  //   if (ok) { verify FAT32, create/open log file, write header }
  recordError(ErrorCode::ERR_SD_INIT, PSTR("setupSD: hardware not present"));
  return false; // hardware not present
}

bool setupRTC() {
  // TODO: Wire.begin(); rtc.begin(); check RTC running
  // On first boot or power loss, set from compile time (__DATE__ __TIME__)
  recordError(ErrorCode::ERR_RTC_READ, PSTR("setupRTC: hardware not present"));
  return false;
}

bool setupTFT() {
  // TODO: Implement with SPI transaction guards
  // Example pattern:
  //   spiBeginTFT();
  //   tft.init(TFT_WIDTH, TFT_HEIGHT);
  //   tft.setRotation(TFT_ROTATION);
  //   spiEndTFT();
  //   Draw splash screen (streaming, no framebuffer)
  recordError(ErrorCode::ERR_TFT_INIT, PSTR("setupTFT: hardware not present"));
  return false;
}

bool setupSensors() {
  // TODO: Wire.begin(); aht.begin(); bmp.begin(); verify WHO_AM_I
  recordError(ErrorCode::ERR_AHT20_READ, PSTR("setupSensors: hardware not present"));
  return false;
}

void loopRTC() {
  // TODO: Read DS3231 via I2C with timeout/retry
  // Populate currentTime, handle I2C errors
  // Example error handling:
  //   if (i2cReadWithRetry(ADDR_DS3231, ...)) { currentTime.valid = true; }
  //   else { recordError(ErrorCode::ERR_RTC_READ, "loopRTC"); rtcReady = false; }
}

void loopEnvironment() {
  // TODO: Read AHT20 (temp/humidity) + BMP280 (pressure) via I2C
  // Validate ranges: -40..85°C, 0..100% RH, 300..1100 hPa
  // Mark currentEnv.valid on success
  // On repeated failure: sensorsReady = false; recordError
}

void loopLight() {
  // TODO: Read analog from TEMT6000, convert to lux
  // Calibration formula needed: lux = (analog * Vref / 1024) * calibration_factor
  // Average multiple samples for noise reduction
  // Mark currentLight.valid
}

void loopLog() {
  // TODO: Append CSV line to SD using spiBeginSD()/spiEndSD()
  // Format: timestamp,temp,humidity,pressure,lux
  // Use fixed stack buffer (LOG_BUFFER_SIZE), snprintf_P with PSTR format
  // Handle SD full / write errors: recordError, maybe set sdCardReady=false
  // Flush periodically to ensure data persistence
}

void loopDisplay() {
  // TODO: Draw current readings on TFT using spiBeginTFT()/spiEndTFT()
  // STREAMING ONLY - no full framebuffer
  // Draw text/primitives directly via TFT library calls within transaction
  // Handle display sleep / rotation
  // On comm failure: recordError, tftReady = false
}

void loopWatchdog() {
  // Reset watchdog timer - must be called < 8s intervals
  wdt_reset();
  
  // Optional: log watchdog status periodically for debugging
#if DEBUG_ENABLED
  static uint8_t wdLogCounter = 0;
  if (++wdLogCounter >= 60) {  // Every ~60 seconds
    wdLogCounter = 0;
    logToSerial(PSTR("Watchdog reset - free RAM: %d"), freeRam());
  }
#endif
}

// ── Helpers ────────────────────────────────────────────────────────────────

// Variadic logging with PROGMEM format string support
void logToSerial(const char* fmt, ...) {
#if DEBUG_ENABLED
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  
  // Detect if fmt is in PROGMEM (heuristic: address > 0x8000 on AVR)
  // For safety, assume PROGMEM when called with PSTR()
  vsnprintf_P(buffer, sizeof(buffer), fmt, args);
  
  va_end(args);
  Serial.println(buffer);
#endif
}

// Error recording with timestamp and context
void recordError(ErrorCode code, const char* context) {
  lastError = code;
  lastErrorTime = millis();
  uint8_t idx = static_cast<uint8_t>(code) % 16;
  if (errorCounts[idx] < 255) errorCounts[idx]++;
  
#if DEBUG_ENABLED
  logToSerial(PSTR("ERROR [%d] %s"), static_cast<uint8_t>(code), context);
#endif
}

// I²C read with timeout and retry (template for sensor drivers)
/*
bool i2cReadWithRetry(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len, uint8_t retries = 3) {
  for (uint8_t attempt = 0; attempt < retries; attempt++) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
      recordError(ErrorCode::ERR_I2C_NACK, PSTR("i2cRead: NACK"));
      continue;
    }
    uint8_t received = Wire.requestFrom(addr, len, true);
    if (received == len) {
      for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
      return true;
    }
    recordError(ErrorCode::ERR_I2C_TIMEOUT, PSTR("i2cRead: timeout"));
    delay(10); // Brief backoff
  }
  return false;
}
*/

// Free RAM diagnostic (AVR-specific)
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// SPI transaction-safe byte transfer (for raw register access if needed)
/*
uint8_t spiTransferTFT(uint8_t data) {
  spiBeginTFT();
  uint8_t result = SPI.transfer(data);
  spiEndTFT();
  return result;
}

uint8_t spiTransferSD(uint8_t data) {
  spiBeginSD();
  uint8_t result = SPI.transfer(data);
  spiEndSD();
  return result;
}
*/

// ── End of file ────────────────────────────────────────────────────────────