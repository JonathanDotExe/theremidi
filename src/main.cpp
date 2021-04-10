#include <Arduino.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

struct CalibrationData {
  uint16_t min_dst = 30;
  uint16_t max_dst = 300;
};

void message(int operation, int channel, uint8_t first_data, uint8_t second_data) {
  uint8_t cmd = (operation & 0xF0) | (channel & 0x0F);
  Serial.write(cmd);
  Serial.write(first_data & 0x7F);
  Serial.write(second_data & 0x7F);
}

void note_on(int channel, int note, int velocity) {
  message(0x90, channel, note, velocity);
}

void note_off(int channel, int note) {
  message(0x80, channel, note, 0);
}

void cc(int channel, int cc, double value) {
  message(0x90, channel, cc, value * 127);
}


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