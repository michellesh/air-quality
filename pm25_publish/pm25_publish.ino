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
#define NUM_AQI_VALUES  120  // 10min * (60s / UPDATE_INTERVAL_SECONDS)
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

struct aqiValue {
  uint16_t aqi;
  unsigned long epochTime;
};

bool wifiConnected = false;
bool adafruitConnected = false;
Timer aqiTimer = {1000 * UPDATE_INTERVAL_SECONDS, 0};
Timer avg10Timer = {1000 * 60 * 10, 0}; // 10 minutes
aqiValue aqiValues[NUM_AQI_VALUES];

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
  if (aqiTimer.complete()) {
    displayAQI();
    aqiTimer.reset();
  }
}

void displayAQI() {
  aqiValue pm25 = getPM25();
  updateAQI(pm25.aqi);

  uint16_t avgPM25 = getAvgPM25(pm25);
  if (avg10Timer.complete()) {
    //display10MinAvg();
    Serial.print("10 min avg (complete): ");
  } else {
    Serial.print("10 min avg (in progress): ");
  }
  Serial.println(avgPM25);
  printAqiValues();

  if (adafruitConnected) {
    aqiFeed->save(pm25.aqi);
  }
}

void printAqiValues() {
  Serial.print("aqiValues = [");
  for (int i = 0; i < NUM_AQI_VALUES; i++) {
    Serial.print(aqiValues[i].aqi);
    if (i < NUM_AQI_VALUES - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
}

uint16_t getAvgPM25(aqiValue pm25) {
  //aqiValue currentValue = {pm25, timeClient.getEpochTime()};
  aqiValue tempAqiValues[NUM_AQI_VALUES] = {pm25};
  int sum = pm25.aqi;
  int count = 1;
  for (int i = 0; i < NUM_AQI_VALUES; i++) {
    if (aqiValues[i].epochTime > 0 && !isOlderThan10min(aqiValues[i].epochTime)) {
      tempAqiValues[count] = aqiValues[i];
      count++;
      sum += aqiValues[i].aqi;
    }
  }
  copyArray(tempAqiValues, aqiValues, NUM_AQI_VALUES);
  return sum / count;
}

void copyArray(aqiValue* src, aqiValue* dst, int len) {
  memcpy(dst, src, sizeof(src[0]) * len);
}

bool isOlderThan10min(unsigned long epochTime) {
  return (timeClient.getEpochTime() - epochTime) > (10 * 60);
}
