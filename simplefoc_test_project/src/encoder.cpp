#include "encoder.h"

void initMa704SSI(int clkPin, int dataPin, int delayUs) {
  pinMode(clkPin, OUTPUT);
  digitalWrite(clkPin, LOW);
  pinMode(dataPin, INPUT);
  delayMicroseconds(delayUs);
}

uint32_t readMa704RawSSI(int clkPin, int dataPin, int bitCount, int delayUs) {
  if (bitCount < 1) {
    bitCount = MA704_DEFAULT_BITS;
  }
  uint32_t value = 0;

  digitalWrite(clkPin, LOW);
  delayMicroseconds(delayUs);

  // First dummy bit cycle (discarded)
  digitalWrite(clkPin, HIGH);
  delayMicroseconds(delayUs);
  digitalWrite(clkPin, LOW);
  delayMicroseconds(delayUs);

  for (int i = 0; i < bitCount; i++) {
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(delayUs);

    value <<= 1;
    value |= digitalRead(dataPin) ? 1u : 0u;

    digitalWrite(clkPin, LOW);
    delayMicroseconds(delayUs);
  }

  return value;
}

float readMa704AngleRad(int clkPin, int dataPin, int bitCount, int delayUs) {
  uint32_t raw = readMa704RawSSI(clkPin, dataPin, bitCount, delayUs);
  uint32_t maxCount = 1u << bitCount;
  return (raw / (float)maxCount) * (2.0f * PI);
}

