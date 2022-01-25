uint16_t getPM25() {
  PM25_AQI_Data data;

  while (!aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(50);  // try again in a bit!
  }

  return data.pm25_standard;
}

void updateAQI(uint16_t pm25) {
  screen.clear();
  displayRect(pm25);
  displayNowText();
  displayAQI(pm25);
  displayTime();
}

void displayRect(uint16_t aqi) {
  display.drawRect(0, 15, 62, 32, SSD1306_WHITE);
}

void displayNowText() {
  screen = screen.textSize(1).x(25).y(5).println("Now");
}

char* intToChars(uint16_t i) {
  char buffer[6];
  sprintf(buffer, "%d", i);
  return buffer;
}

void displayAQI(uint16_t aqi) {
  if (aqi >= 1000) {
    aqi = 999;
  }
  screen = screen.textSize(3)
                 .x(aqi < 10 ? 25 : aqi < 100 ? 15 : 5).y(20)
                 .println(intToChars(aqi));
}

void displayTime() {
  int hours, minutes, seconds;
  if (wifiConnected) {
    hours = timeClient.getHours();
    minutes = timeClient.getMinutes();
    seconds = timeClient.getSeconds();
  } else {
    hours = minutes = seconds = 0;
  }

  char buffer[30];
  sprintf(buffer, "%d:%02d:%02d", hours, minutes, seconds);

  screen = screen.textSize(1)
                 .x(hours < 10 ? 12 : 8).y(50)
                 .println(buffer);
}
