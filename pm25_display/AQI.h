struct AQI {
  uint16_t value;
  unsigned long atMillis;
};

// A ring buffer of AQI values.
class AqiBuffer {
 public:
  AqiBuffer(int size) {
    aqis.resize(size);
  }

  // Inserts a new value, overwriting the oldest value if the ring buffer is
  // full.
  //
  // Inserted values should always be younger than all values already in
  // the buffer.
  void insert(AQI newAqi) {
    latest = (latest + 1) % aqis.size();
    aqis[latest] = newAqi;
  }

  // Returns the average of all values younger than maxAgeMs.
  int average(unsigned long maxAgeMs) {
    int sum = 0;
    int count = 0;
    int i = latest;
    unsigned long nowMillis = millis();
    while (aqis[i].atMillis != 0 && nowMillis - aqis[i].atMillis < maxAgeMs) {
      sum += aqis[i].value;
      ++count;

      // Maybe I forgot how negative modulo works, but it wasn't doing what I
      // expected, so I just add aqis.size() to keep everything positive.
      i = (i - 1 + aqis.size()) % aqis.size();
      if (i == latest) {
        break;
      }
    }
    return sum / count;
  }

 private:
  std::vector<AQI> aqis;
  int latest = 0;
};
