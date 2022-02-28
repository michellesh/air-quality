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

  Screen y(int16_t y) {
    _y = y;
    display.setCursor(_x, y);
    return *this;
  }

  void print(auto str) {
    Serial.print(str);
    display.print(str);
    display.display();
  }

  void println(auto str) {
    Serial.println(str);
    display.println(str);
    display.display();
  }

  void clear() {
    display.clearDisplay();
    display.display();
  }
};
