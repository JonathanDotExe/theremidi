#include <Arduino.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

struct CalibrationData {
  uint16_t min_dst = 30;
  uint16_t max_dst = 300;
};

void message(int operation, int channel, uint8_t first_data, uint8_t second_data) {
  int cmd = (operation & 0xF0) | (channel & 0x0F);
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
  message(0xB0, channel, cc, value * 127);
}

void pitch_bend(int channel, double value) {
  int val = round(16383 * value);
  int first_byte = val & 0x7F;
  int second_byte = (val >> 7) & 0x7F;
  message(0xE0, channel, first_byte, second_byte);
}

CalibrationData calibration;


enum ThereMidiMode {
  CC_MODE, PITCH_BEND_MODE
};

struct CCModePreset {
  int cc = 1;
};

struct PitchBendModePreset {
  double start_bend = 0.5;
  bool bending = false;
};

struct ThereMidiPreset {
  int channel = 0;
  ThereMidiMode mode = CC_MODE;
  CCModePreset cc;
  PitchBendModePreset pb;
};

ThereMidiPreset preset;

void setup() {
  Serial.begin(115200); //MIDI baud rate

  //Wait for serial
  while (! Serial) {
    delay(1);
  }

  //Init sensor
  if (!lox.begin()) {
    Serial.println("Failed to initialize sensor");
    while (1); //Sleep foreever
  }
}

void loop() {
  VL53L0X_RangingMeasurementData_t data;
  lox.rangingTest(&data, false);

  switch (preset.mode) {
    case CC_MODE:
      //Send cc
      if (data.RangeStatus != 4) {
        cc(preset.channel, preset.cc.cc, fmin(fmax(0, (double) (data.RangeMilliMeter - calibration.min_dst)/calibration.max_dst), 1));
      }
      break;
    case PITCH_BEND_MODE:
      //Send pitch bend
      if (data.RangeStatus != 4) {
        double val = fmin(fmax(0, (double) (data.RangeMilliMeter - calibration.min_dst)/calibration.max_dst), 1);
        if (!preset.pb.bending) {
          preset.pb.bending = true;
          preset.pb.start_bend = val;
        }
        pitch_bend(preset.channel, fmax(0, fmin(1, (val - preset.pb.start_bend) * 3 + 0.5)));
      }
      else if (preset.pb.bending) {
        pitch_bend(preset.channel, 0.5);
        preset.pb.bending = false;
      }
      break;
  }

}