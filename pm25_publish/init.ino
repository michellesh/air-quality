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
  if (!aqi.begin_UART(&pmSerial)) {
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("PM25 found!");
}

void displayConnectingTo(char* name) {
  screen = screen.x(5).y(5)
                 .textSize(1)
                 .println("Connecting to ")
                 .incY()
                 .println(name)
                 .incY();
}

bool connectWiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  displayConnectingTo(SECRET_SSID);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    screen = screen.print(".").incX();
    if (screen.getX() >= SCREEN_WIDTH) {
      screen = screen.clear();
      displayConnectingTo(SECRET_SSID);
    }
    attempts++;
  }

  wifiConnected = WiFi.status() == WL_CONNECTED;
  displayConnected(wifiConnected);
}

void connectAdafruitIO() {
  io.connect();

  displayConnectingTo("Adafruit IO");

  int attempts = 0;
  while (io.status() < AIO_CONNECTED && attempts < 10) {
    delay(500);
    screen = screen.print(".").incX();
    if (screen.getX() >= SCREEN_WIDTH) {
      screen = screen.clear();
      displayConnectingTo("Adafruit IO");
    }
    attempts++;
  }

  adafruitConnected = io.status() >= AIO_CONNECTED;
  displayConnected(adafruitConnected);
}

void displayConnected(bool isConnected) {
  screen = screen.x(5).incY().incY();
  if (isConnected) {
    screen.println("Connected! :)");
  } else {
    screen.println("Failed to connect :(");
  }
  delay(3000);
  screen = screen.clear();
}
