#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

// MA704 SSI encoder helper functions for ESP32.
// These helpers use software bit-banging only.
// MA704 sends one dummy clock cycle first, then 16 valid data bits.

constexpr int MA704_DEFAULT_BITS = 16;
constexpr int MA704_DEFAULT_DELAY_US = 1;

// Configure SSI pins for the MA704 using bit-banging.
void initMa704SSI(int clkPin, int dataPin, int delayUs = MA704_DEFAULT_DELAY_US);

// Read raw MA704 SSI counts using bit-banged GPIO.
// Returned value is [0..(2^bitCount)-1].
uint32_t readMa704RawSSI(int clkPin, int dataPin, int bitCount = MA704_DEFAULT_BITS, int delayUs = MA704_DEFAULT_DELAY_US);

// Convert raw MA704 SSI counts into radians [0..2*PI) using bit-bang read.
float readMa704AngleRad(int clkPin, int dataPin, int bitCount = MA704_DEFAULT_BITS, int delayUs = MA704_DEFAULT_DELAY_US);

#endif // ENCODER_H
