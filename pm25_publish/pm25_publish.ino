#include "config.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "AQI.h"
#include "Timer.h"

// AQI sensor
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqiSensor = Adafruit_PM25AQI();

// Adafruit IO
AdafruitIO_Feed *aqiFeed = io.feed("Base Camp AQI");

// OLED screen
#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NTP for getting current time
const long utcOffsetInSeconds = 6 * 3600; // Your time zone
WiFiUDP ntpUDP; // Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// other globals
#define UPDATE_INTERVAL_SECONDS  5  // Every X seconds, read sensor and update screen
#define NUM_AQI_VALUES  120  // 10min * (60s / UPDATE_INTERVAL_SECONDS)


bool wifiConnected = false;
bool adafruitConnected = false;
Timer timerReadSensor = {1000 * UPDATE_INTERVAL_SECONDS, 0};
Timer timerStartAvg = {1000 * 60 * 10, 0}; // 10 minutes
AQI aqiValues[NUM_AQI_VALUES];

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
  if (timerReadSensor.complete()) {
    displayAQI();
    timerReadSensor.reset();
  }
}

void displayAQI() {
  AQI aqi = readAqiSensor();
  updateAQI(aqi);

  uint16_t avg = getAvg(aqi);
  if (timerStartAvg.complete()) {
    //display10MinAvg();
    Serial.print("10 min avg (complete): ");
  } else {
    Serial.print("10 min avg (in progress): ");
  }
  Serial.println(avg);
  printAqiValues();

  if (adafruitConnected) {
    aqiFeed->save(aqi.value);
  }
}

void printAqiValues() {
  Serial.print("aqiValues = [");
  for (int i = 0; i < NUM_AQI_VALUES; i++) {
    Serial.print(aqiValues[i].value);
    if (i < NUM_AQI_VALUES - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
}

uint16_t getAvg(AQI aqi) {
  AQI tempAqiValues[NUM_AQI_VALUES] = {aqi};
  int sum = aqi.value;
  int count = 1;
  for (int i = 0; i < NUM_AQI_VALUES; i++) {
    if (aqiValues[i].epochTime > 0 && !isOlderThan10min(aqiValues[i].epochTime)) {
      tempAqiValues[count] = aqiValues[i];
      count++;
      sum += aqiValues[i].value;
    }
  }
  copyArray(tempAqiValues, aqiValues, NUM_AQI_VALUES);
  return sum / count;
}
