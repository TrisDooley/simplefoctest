#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <SPI.h>

// MA704 SSI encoder helper functions for ESP32.
// These helpers can use either software bit-banging or the ESP32 SPI peripheral.

constexpr int MA704_DEFAULT_BITS = 17;
constexpr int MA704_DEFAULT_DELAY_US = 1;
constexpr long MA704_DEFAULT_CLOCK = 1000000L;

// Configure SSI pins for the MA704 using bit-banging.
void initMa704SSI(int clkPin, int dataPin, int delayUs = MA704_DEFAULT_DELAY_US);

// Configure SSI pins for the MA704 using the ESP32 hardware SPI peripheral.
void initMa704SSISPI(SPIClass& spi, int clkPin, int dataPin);

// Read raw MA704 SSI counts using bit-banged GPIO.
// Returned value is [0..(2^bitCount)-1].
uint32_t readMa704RawSSI(int clkPin, int dataPin, int bitCount = MA704_DEFAULT_BITS, int delayUs = MA704_DEFAULT_DELAY_US);

// Read raw MA704 SSI counts using ESP32 hardware SPI.
uint32_t readMa704RawSSISPI(SPIClass& spi, int bitCount = MA704_DEFAULT_BITS, long clockSpeed = MA704_DEFAULT_CLOCK, int delayUs = MA704_DEFAULT_DELAY_US);

// Convert raw MA704 SSI counts into radians [0..2*PI) using bit-bang read.
float readMa704AngleRad(int clkPin, int dataPin, int bitCount = MA704_DEFAULT_BITS, int delayUs = MA704_DEFAULT_DELAY_US);

// Convert raw MA704 SSI counts into radians [0..2*PI) using hardware SPI read.
float readMa704AngleRadSSISPI(SPIClass& spi, int bitCount = MA704_DEFAULT_BITS, long clockSpeed = MA704_DEFAULT_CLOCK, int delayUs = MA704_DEFAULT_DELAY_US);

// Convert raw MA704 SSI counts into degrees [0..360) using bit-bang read.
float readMa704AngleDeg(int clkPin, int dataPin, int bitCount = MA704_DEFAULT_BITS, int delayUs = MA704_DEFAULT_DELAY_US);

// Convert raw MA704 SSI counts into degrees [0..360) using hardware SPI read.
float readMa704AngleDegSSISPI(SPIClass& spi, int bitCount = MA704_DEFAULT_BITS, long clockSpeed = MA704_DEFAULT_CLOCK, int delayUs = MA704_DEFAULT_DELAY_US);

#endif // ENCODER_H
