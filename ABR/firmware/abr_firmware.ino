/**
 * ABR Weather Station — Firmware Skeleton
 * Phase 2: Basic structure, non-blocking loop.
 * Sensor drivers added in Phase 2 (when hardware available).
 */

#include "config.h"

// ── Libraries (add as sensors are implemented) ─────────────────────────────
// #include <Wire.h>           // I²C
// #include <SPI.h>            // SPI
// #include <SD.h>             // MicroSD
// #include <Adafruit_ST7789.h> // TFT
// #include <Adafruit_AHTX0.h> // AHT20
// #include <Adafruit_BMP280.h> // BMP280
// #include <RTClib.h>         // DS3231

// ── Global state ───────────────────────────────────────────────────────────
unsigned long lastRtcRead    = 0;
unsigned long lastEnvRead    = 0;
unsigned long lastLightRead  = 0;
unsigned long lastLogWrite   = 0;
unsigned long lastDisplay    = 0;

bool sdCardReady   = false;
bool rtcReady      = false;
bool tftReady      = false;
bool sensorsReady  = false;

// ── Sensor data holders ────────────────────────────────────────────────────
struct EnvReading {
  float temperature = NAN;
  float humidity    = NAN;
  float pressure    = NAN;
  bool  valid       = false;
} currentEnv;

struct LightReading {
  float lux         = NAN;
  bool  valid       = false;
} currentLight;

struct DateTime {
  uint16_t year = 0;
  uint8_t  month = 0, day = 0, hour = 0, minute = 0, second = 0;
  bool     valid = false;
} currentTime;

// ── Forward declarations ───────────────────────────────────────────────────
void setupSerial();
void setupPins();
bool setupSD();
bool setupRTC();
bool setupTFT();
bool setupSensors();
void loopRTC();
void loopEnvironment();
void loopLight();
void loopLog();
void loopDisplay();
void logToSerial(const char* fmt, ...);

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  setupSerial();
  setupPins();

  sdCardReady  = setupSD();
  rtcReady     = setupRTC();
  tftReady     = setupTFT();
  sensorsReady = setupSensors();

  logToSerial("ABR firmware skeleton initialised");
  logToSerial("SD: %s  RTC: %s  TFT: %s  Sensors: %s",
              sdCardReady ? "OK" : "FAIL",
              rtcReady ? "OK" : "FAIL",
              tftReady ? "OK" : "FAIL",
              sensorsReady ? "OK" : "FAIL");
}

// ── Main loop (non-blocking) ───────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  if (rtcReady     && now - lastRtcRead   >= INTERVAL_RTC)     { loopRTC();      lastRtcRead   = now; }
  if (sensorsReady && now - lastEnvRead   >= INTERVAL_ENV)     { loopEnvironment(); lastEnvRead  = now; }
  if (sensorsReady && now - lastLightRead >= INTERVAL_LIGHT)   { loopLight();     lastLightRead = now; }
  if (sdCardReady  && now - lastLogWrite  >= INTERVAL_LOG)     { loopLog();       lastLogWrite  = now; }
  if (tftReady     && now - lastDisplay   >= INTERVAL_DISPLAY) { loopDisplay();   lastDisplay   = now; }
}

// ── Module stubs (implement when hardware arrives) ─────────────────────────

void setupSerial() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial && millis() < 2000) { /* wait for USB serial */ }
}

void setupPins() {
  pinMode(PIN_TFT_CS,   OUTPUT); digitalWrite(PIN_TFT_CS,   HIGH);
  pinMode(PIN_TFT_DC,   OUTPUT);
  pinMode(PIN_TFT_RST,  OUTPUT);
  pinMode(PIN_SD_CS,    OUTPUT); digitalWrite(PIN_SD_CS,    HIGH);
  pinMode(PIN_LIGHT_OUT, INPUT);
  // I²C and SPI pins handled by libraries
}

bool setupSD() {
  // TODO: SD.begin(PIN_SD_CS);
  // TODO: Verify card type, FAT32, create log file with header
  return false; // hardware not present
}

bool setupRTC() {
  // TODO: Wire.begin(); rtc.begin(); check RTC running, set from compile time if needed
  return false;
}

bool setupTFT() {
  // TODO: tft.init(TFT_WIDTH, TFT_HEIGHT); tft.setRotation(TFT_ROTATION); splash screen
  return false;
}

bool setupSensors() {
  // TODO: aht.begin(); bmp.begin(); verify IDs
  return false;
}

void loopRTC() {
  // TODO: Read DS3231, populate currentTime
  // TODO: Handle RTC failure gracefully
}

void loopEnvironment() {
  // TODO: Read AHT20 (temp/humidity) + BMP280 (pressure)
  // TODO: Validate ranges, mark currentEnv.valid
}

void loopLight() {
  // TODO: Read analog from TEMT6000, convert to lux (calibration needed)
  // TODO: Mark currentLight.valid
}

void loopLog() {
  // TODO: Append CSV line to SD: timestamp,temp,humidity,pressure,lux
  // TODO: Handle SD full / write errors
}

void loopDisplay() {
  // TODO: Draw current readings on TFT
  // TODO: Handle display sleep / rotation
}

// ── Helpers ────────────────────────────────────────────────────────────────
void logToSerial(const char* fmt, ...) {
#if DEBUG_ENABLED
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  Serial.println(buffer);
#endif
}