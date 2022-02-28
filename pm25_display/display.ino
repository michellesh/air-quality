void displayNowAqi() {
  screen.clear();
  displayRect();
  displayWhenText();
  displayAQI();
}

void displayAvgAqi() {
  screen.textSize(1).x(70).y(5).println("10m avg");
  display.drawLine(70, 15, 110, 15, SSD1306_WHITE);
  screen = screen.textSize(2).x(70).y(23);

  if (timerStartAvg.complete()) {
    screen.println(getAvg());
  } else {
    screen.println("--");
  }
}

void displayRect() {
  display.drawRect(0, 15, 62, 32, SSD1306_WHITE);
}

void displayWhenText() {
  unsigned long timeSinceLastData = millis() - aqi.atMillis;

  if (timeSinceLastData < seconds(10)) {
    screen.textSize(1).x(25).y(5).println("Now");
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
  screen.textSize(1).x(12).y(5).println(buffer);
}

void displayAQI() {
  if (aqi.value >= 1000) {
    aqi.value = 999;
  }
  screen.textSize(3)
        .x(aqi.value < 10 ? 25 : aqi.value < 100 ? 15 : 5)
        .y(20)
        .println(aqi.value);
}
