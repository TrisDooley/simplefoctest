#include <Arduino.h>
#include "encoder.h"

// Example bit-bang pins for ESP32 (change to your wiring)
const int MA704_CLK_1 = 18;   // clock output
const int MA704_DATA_1 = 19;  // data input

const int MA704_CLK_2 = 5;   // clock output
const int MA704_DATA_2 = 23;  // data input

void setup() {
  Serial.begin(115200);
  initMa704SSI(MA704_CLK_1, MA704_DATA_1);
  initMa704SSI(MA704_CLK_2, MA704_DATA_2);
  Serial.println("MA704 SSI bit-bang reader ready");
}

void loop() {
  float angle = readMa704AngleRad(MA704_CLK_1, MA704_DATA_1);
  float angle2 = readMa704AngleRad(MA704_CLK_2, MA704_DATA_2);
  Serial.print("Angle1: ");
  Serial.print(angle);
  Serial.print(" rad, Angle2: ");
  Serial.print(angle2);
  Serial.println(" rad");
  delay(100); // read every second
}
