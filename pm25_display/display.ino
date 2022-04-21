void displayNowAqi() {
  screen.clear();
  displayRect();
  displayWhenText();
  displayAQI();
  displayNiceMessage();
}

void displayAvgAqi() {
  screen.textSize(1).x(70).y(8).println("10m avg");
  display.drawLine(70, 16, 110, 16, SSD1306_WHITE);
  screen = screen.textSize(2).x(70).y(24);

  if (timerStartAvg.complete()) {
    screen.println(aqiValues.average(minutes(10)));
  } else {
    screen.println("--");
  }
}

void displayRect() {
  display.drawRect(0, 16, 62, 33, SSD1306_WHITE);
}

void displayWhenText() {
  unsigned long timeSinceLastData = millis() - aqi.atMillis;

  if (timeSinceLastData < seconds(10)) {
    screen.textSize(1).x(25).y(8).println("Now");
    return;
  }

  char buffer[30];
  if (timeSinceLastData < seconds(60)) {
    sprintf(buffer, "%ds ago", msToSeconds(timeSinceLastData));
  } else if (timeSinceLastData < minutes(60)) {
    sprintf(buffer, "%dm ago", msToMinutes(timeSinceLastData));
  } else if (timeSinceLastData < hours(24)) {
    sprintf(buffer, "%dh ago", msToHours(timeSinceLastData));
  } else {
    sprintf(buffer, "%dd ago", msToDays(timeSinceLastData));
  }
  screen.textSize(1).x(12).y(8).println(buffer);
}

void displayAQI() {
  uint16_t aqiValue = aqi.value >= 1000 ? 999 : aqi.value;
  screen.textSize(3)
        .x(aqiValue < 10 ? 25 : aqiValue < 100 ? 15 : 5)
        .y(21)
        .println(aqiValue);
}

void displayNiceMessage() {
  screen.textSize(1).x(4).y(51).println("Have a lovely day :)");
}
