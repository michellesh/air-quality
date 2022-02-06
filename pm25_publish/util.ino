void copyArray(AQI* src, AQI* dst, int len) {
  memcpy(dst, src, sizeof(src[0]) * len);
}

char* intToChars(uint16_t i) {
  char buffer[6];
  sprintf(buffer, "%d", i);
  return buffer;
}

bool isOlderThan10min(unsigned long epochTime) {
  return (timeClient.getEpochTime() - epochTime) > (10 * 60);
}
