#include "config.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>

SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

AdafruitIO_Feed *aqiFeed = io.feed("Base Camp AQI");

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  pmSerial.begin(9600);

  connectSensor();
  connectAdafruitIO();
}

void loop() {
  io.run();

  PM25_AQI_Data data;

  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(50);  // try again in a bit!
    return;
  }

  Serial.print("PM 2.5: ");
  Serial.println(data.pm25_standard);

  aqiFeed->save(data.pm25_standard);

  delay(5000);
}

void connectSensor() {
  // Wait one second for sensor to boot up!
  delay(1000);

  // connect to the sensor over software serial
  if (! aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
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
