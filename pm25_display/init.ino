void initDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.setTextColor(SSD1306_WHITE);
  //display.display();
  delay(2000);
}

void initSensor() {
  // Wait one second for sensor to boot up!
  delay(1000);

  // connect to the sensor over software serial
  if (!aqiSensor.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}
