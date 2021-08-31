// code source for reading pm2.5 sensor:
// https://how2electronics.com/iot-air-pollution-monitoring-esp8266/

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Arduino.h>

#define LENG 31  // 0x42 + 31 bytes equal to 32 bytes

unsigned char buf[LENG];
int pm25Value = 0; // define PM2.5 value of the air detector module

void setup() {
  Serial.begin(9600);
  delay(10);
}

void loop() {
  if (Serial.find(0x42)) {    // start to read when detect 0x42
    Serial.readBytes(buf, LENG);

    if (buf[0] == 0x4d) {
      if (checkValue(buf, LENG)) {
        pm25Value = transmitPM25(buf);// count PM2.5 value of the air detector module

        Serial.print("PM2.5: ");
        Serial.print(pm25Value);
        Serial.println("  ug/m3");
      }
    }
  }
  delay(100);
}

char checkValue(unsigned char *thebuf, char leng) {
  char receiveflag = 0;
  int receiveSum = 0;

  for (int i = 0; i < (leng - 2); i++) {
    receiveSum = receiveSum + thebuf[i];
  }
  receiveSum = receiveSum + 0x42;

  if (receiveSum == ((thebuf[leng - 2] << 8) + thebuf[leng - 1])) {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

int transmitPM25(unsigned char *thebuf) {
  int pm25Val;
  pm25Val = ((thebuf[5] << 8) + thebuf[6]); // count PM2.5 value of the air detector module
  return pm25Val;
}
