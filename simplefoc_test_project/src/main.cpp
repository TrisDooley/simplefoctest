#include <Arduino.h>
#include <SPI.h>
#include "encoder.h"

void setup() {
  Serial.begin(115200);
  initMa704SSISPI(SPI, 18, 19); // example pins: SCK=18, MISO=19
  float angle = readMa704AngleRadSSISPI(SPI);
  Serial.println(angle);
}