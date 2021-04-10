#include <Arduino.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  //Wait for serial
  while (! Serial) {
    delay(1);
  }

  if (!lox.begin()) {
    Serial.println("Failed to initialize sensor");
    while (1); //Sleep foreever
  }
}

void loop() {
  VL53L0X_RangingMeasurementData_t data;
  lox.rangingTest(&data, false);

  if (data.RangeStatus != 4) {
    Serial.println(data.RangeMilliMeter);
  }
  else {
    Serial.println("out of range");
  }
}