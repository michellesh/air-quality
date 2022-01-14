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

SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

AdafruitIO_Feed *aqiFeed = io.feed("Base Camp AQI");

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long utcOffsetInSeconds = 6 * 3600; // Your time zone
WiFiUDP ntpUDP; // Define NTP Client to get time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);

  initDisplay();
  initSensor();
  connectWiFi();
  connectAdafruitIO();
  timeClient.begin();
}

void loop() {
  io.run();
  display.clearDisplay();
  timeClient.update();

  PM25_AQI_Data data;

  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(50);  // try again in a bit!
    return;
  }

  Serial.print("PM 2.5: ");
  Serial.println(data.pm25_standard);

  aqiFeed->save(data.pm25_standard);

  displayRect(display, data.pm25_standard);
  displayNowAQI(display, data.pm25_standard);
  displayTime(display);

  display.display();
  delay(2000);
}

void displayRect(Adafruit_SSD1306 &d, uint16_t aqi) {
  display.drawRect(0, 15, 62, 32, SSD1306_WHITE);
}

void displayTime(Adafruit_SSD1306 &d) {
  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();
  char buffer[30];
  sprintf(buffer, "%d:%02d:%02d", hours, minutes, seconds);
  Serial.print("Last Updated: ");
  Serial.println(buffer);

  d.setTextSize(1);
  d.setCursor(7, 50);
  d.println(buffer);
}

void displayNowAQI(Adafruit_SSD1306 &d, uint16_t aqi) {
  d.setTextSize(3);
  if (aqi < 10) {
    d.setCursor(25, 20);
  } else if (aqi < 100) {
    d.setCursor(15, 20);
  } else if (aqi < 1000) {
    d.setCursor(5, 20);
  } else {
    aqi = 999;
  }
  char buffer[16];
  sprintf(buffer, "%d", aqi);
  d.println(buffer);
}

void initSensor() {
  // Wait one second for sensor to boot up!
  delay(1000);

  // connect to the sensor over software serial
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}

void connectAdafruitIO() {
  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void connectWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(2000);
}
