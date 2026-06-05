#include "encoder.h"

void initMa704SSI(int clkPin, int dataPin, int delayUs) {
  pinMode(clkPin, OUTPUT);
  digitalWrite(clkPin, LOW);
  pinMode(dataPin, INPUT_PULLUP);
  delayMicroseconds(delayUs);
}

void initMa704SSISPI(SPIClass& spi, int clkPin, int dataPin) {
  pinMode(clkPin, OUTPUT);
  digitalWrite(clkPin, LOW);
  pinMode(dataPin, INPUT_PULLUP);
  spi.begin(clkPin, dataPin, -1, -1);
}

uint32_t readMa704RawSSI(int clkPin, int dataPin, int bitCount, int delayUs) {
  if (bitCount < 1) {
    bitCount = MA704_DEFAULT_BITS;
  }
  uint32_t value = 0;

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

uint32_t readMa704RawSSISPI(SPIClass& spi, int bitCount, long clockSpeed, int delayUs) {
  if (bitCount < 1) {
    bitCount = MA704_DEFAULT_BITS;
  }
  if (bitCount > 32) {
    bitCount = 32;
  }

  const int bytes = (bitCount + 7) / 8;
  const SPISettings settings(clockSpeed, MSBFIRST, SPI_MODE0);
  uint32_t value = 0;

  delayMicroseconds(delayUs);
  spi.beginTransaction(settings);
  for (int i = 0; i < bytes; i++) {
    value = (value << 8) | spi.transfer(0x00);
  }
  spi.endTransaction();

  if (bytes * 8 != bitCount) {
    value >>= (bytes * 8 - bitCount);
  }

  const uint32_t mask = (bitCount >= 32) ? 0xFFFFFFFFu : ((1u << bitCount) - 1u);
  return value & mask;
}

float readMa704AngleRad(int clkPin, int dataPin, int bitCount, int delayUs) {
  uint32_t raw = readMa704RawSSI(clkPin, dataPin, bitCount, delayUs);
  uint32_t maxCount = 1u << bitCount;
  return (raw / (float)maxCount) * (2.0f * PI);
}

float readMa704AngleRadSSISPI(SPIClass& spi, int bitCount, long clockSpeed, int delayUs) {
  uint32_t raw = readMa704RawSSISPI(spi, bitCount, clockSpeed, delayUs);
  uint32_t maxCount = 1u << bitCount;
  return (raw / (float)maxCount) * (2.0f * PI);
}

float readMa704AngleDeg(int clkPin, int dataPin, int bitCount, int delayUs) {
  return readMa704AngleRad(clkPin, dataPin, bitCount, delayUs) * (180.0f / PI);
}

float readMa704AngleDegSSISPI(SPIClass& spi, int bitCount, long clockSpeed, int delayUs) {
  return readMa704AngleRadSSISPI(spi, bitCount, clockSpeed, delayUs) * (180.0f / PI);
}

