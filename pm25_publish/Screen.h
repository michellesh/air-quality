struct Screen {
  uint8_t _textSize = 1;
  int16_t _x = 0;
  int16_t _y = 0;

  Screen textSize(uint8_t textSize) {
    _textSize = textSize;
    display.setTextSize(textSize);
    return *this;
  }

  Screen x(int16_t x) {
    _x = x;
    display.setCursor(x, _y);
    return *this;
  }

  int16_t getX() {
    return _x;
  }

  Screen y(int16_t y) {
    _y = y;
    display.setCursor(_x, y);
    return *this;
  }

  Screen incX() {
    _x = _x + 5;
    display.setCursor(_x, _y);
    return *this;
  }

  Screen incY() {
    _y = _y + 10;
    display.setCursor(_x, _y);
    return *this;
  }

  Screen print(char* str) {
    Serial.print(str);
    display.print(str);
    display.display();
    return *this;
  }

  Screen println(char* str) {
    Serial.println(str);
    display.println(str);
    display.display();
    return *this;
  }

  Screen clear() {
    display.clearDisplay();
    display.display();
    return *this;
  }
};
