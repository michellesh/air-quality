#include "config.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ESP8266 pin number -> GPIO mapping
#define D0  16
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15

#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define UPDATE_INTERVAL_SECONDS  5  // Every X seconds, read sensor and update screen

SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

AdafruitIO_Feed *aqiFeed = io.feed("Base Camp AQI");

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long utcOffsetInSeconds = 6 * 3600; // Your time zone
WiFiUDP ntpUDP; // Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

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

Timer timer = {1000 * UPDATE_INTERVAL_SECONDS, 0};

#include "Screen.h"
uint8_t DEFAULT_TEXT_SIZE = 1;
int16_t DEFAULT_X = 0;
int16_t DEFAULT_Y = 0;
Screen screen = {DEFAULT_TEXT_SIZE, DEFAULT_X, DEFAULT_Y};

bool wifiConnected = false;

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
  if (wifiConnected) {
    io.run();
    timeClient.update();
  }

  if (timer.complete()) {
    displayAQI();
    timer.reset();
  }
}

void displayAQI() {
  screen.clear();
  uint16_t pm25 = getPM25();
  aqiFeed->save(pm25);
  updateAQI(pm25);
}
