void displayNowAqi(AQI aqi) {
  screen.clear();
  displayRect(aqi.value);
  displayNowText();
  displayAQI(aqi.value);
  displayTime();
}

void displayAvgAqi(uint16_t avg) {
  screen = screen.textSize(1).x(70).y(5).println("10m avg");
  display.drawLine(70, 15, 110, 15, SSD1306_WHITE);
  char* chars = timerStartAvg.complete() ? intToChars(avg) : (char*)"--";
  screen = screen.textSize(2).x(70).y(23).println(chars);
}

void displayRect(uint16_t aqi) {
  display.drawRect(0, 15, 62, 32, SSD1306_WHITE);
}

void displayNowText() {
  screen = screen.textSize(1).x(25).y(5).println("Now");
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
