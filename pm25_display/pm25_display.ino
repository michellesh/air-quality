#include "config.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "AQI.h"
#include "Timer.h"

// AQI sensor
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqiSensor = Adafruit_PM25AQI();

// OLED screen
#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// other globals
#define UPDATE_INTERVAL_SECONDS  5  // Every X seconds, read sensor and update screen
#define NUM_AQI_VALUES  120  // 10min * (60s / UPDATE_INTERVAL_SECONDS)

// Timers
Timer timerReadSensor = {1000 * UPDATE_INTERVAL_SECONDS, 0};
Timer timerStartAvg = {1000 * 60 * 10, 0}; // 10 minutes

AqiBuffer aqiValues(NUM_AQI_VALUES);

#include "Screen.h"
Screen screen;
AQI aqi;

void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);

  initDisplay();
  initSensor();
}

void loop() {
  if (timerReadSensor.complete()) {
    timerReadSensor.reset();

    readAqiSensor();
    aqiValues.insert(aqi);

    displayNowAqi();
    displayAvgAqi();
  }
}

void readAqiSensor() {
  // Clear the serial buffer.
  while (pmSerial.available()) {
    pmSerial.read();
  }

  // Wait for a reading.
  unsigned long start_ms = millis();
  PM25_AQI_Data data;
  while (millis() - start_ms < seconds(2)) {
    if (aqiSensor.read(&data)) {
      aqi = {data.pm25_standard, millis()};
      break;
    }
  }
}
