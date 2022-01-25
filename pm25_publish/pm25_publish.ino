#include "config.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// AQI sensor
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// Adafruit IO
AdafruitIO_Feed *aqiFeed = io.feed("Base Camp AQI");

// OLED screen
#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// For getting current time
const long utcOffsetInSeconds = 6 * 3600; // Your time zone
WiFiUDP ntpUDP; // Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// other stuff
#define UPDATE_INTERVAL_SECONDS  5  // Every X seconds, read sensor and update screen
struct Timer {
  unsigned long totalCycleTime;
  unsigned long lastCycleTime;
  void reset() {
    lastCycleTime = millis();
  };
  bool complete() {
    return (millis() - lastCycleTime) > totalCycleTime;
  };
};

bool wifiConnected = false;
bool adafruitConnected = false;
Timer timer = {1000 * UPDATE_INTERVAL_SECONDS, 0};

#include "Screen.h"
Screen screen;

void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);

  initDisplay();
  initSensor();
  connectWiFi();
  if (wifiConnected) {
    connectAdafruitIO();
    timeClient.begin();
  }
}

void loop() {
  if (adafruitConnected) {
    io.run();
  }
  if (wifiConnected) {
    timeClient.update();
  }
  if (timer.complete()) {
    displayAQI();
    timer.reset();
  }
}

void displayAQI() {
  uint16_t pm25 = getPM25();
  updateAQI(pm25);
  if (adafruitConnected) {
    aqiFeed->save(pm25);
  }
}
