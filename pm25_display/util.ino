void copyArray(AQI* src, AQI* dst, int len) {
  memcpy(dst, src, sizeof(src[0]) * len);
}

unsigned long seconds(int s) {
  return s * 1000;
}

unsigned long minutes(int m) {
  return m * 1000 * 60;
}

unsigned long hours(int h) {
  return h * 1000 * 60 * 60;
}

int msToSeconds(unsigned long ms) {
  return ms / 1000;
}

int msToMinutes(unsigned long ms) {
  return ms / 1000 / 60;
}

int msToHours(unsigned long ms) {
  return ms / 1000 / 60 / 60;
}

int msToDays(unsigned long ms) {
  return ms / 1000 / 60 / 60 / 24;
}
