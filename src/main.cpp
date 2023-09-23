#include <Arduino.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

struct CalibrationData {
  uint16_t min_dst = 30;
  uint16_t max_dst = 400;
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
  CC_MODE, PITCH_BEND_MODE, NOTE_MODE
};

struct CCModeData {
  int cc = 1;
};

struct PitchBendModeData {
  double start_bend = 0.5;
  bool bending = false;
};

struct NoteModeData {
  int min_note = 68;
  int max_note = 82;
  bool quantize = true;
  double jitter_threshold = 0.15;

  bool playing = false;
  int curr_note = 61;
};

struct ThereMidiData {
  int channel = 0;
  ThereMidiMode mode = ThereMidiMode::NOTE_MODE;
  CCModeData cc;
  PitchBendModeData pb;
  NoteModeData note;
};

ThereMidiData preset;

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
          note_on(0, 60, 127);
          preset.pb.bending = true;
          preset.pb.start_bend = val;
        }
        pitch_bend(preset.channel, fmax(0, fmin(1, (val - preset.pb.start_bend) * 3 + 0.5)));
      }
      else if (preset.pb.bending) {
        pitch_bend(preset.channel, 0.5);
        note_off(0, 60);
        preset.pb.bending = false;
      }
      break;
    case NOTE_MODE:
      if (data.RangeStatus != 4) {
        //Found data
        double val = fmin(fmax(0, (double) (data.RangeMilliMeter - calibration.min_dst)/calibration.max_dst), 1)  * (preset.note.max_note - preset.note.min_note) + preset.note.min_note;
        int note = round(val);
        //No note is playing
        //Play new note
        if (!preset.note.playing) {
          preset.note.curr_note = note;
          preset.note.playing = true;
          note_on(0, note, 127);
        }
        //Change note
        else if (note != preset.note.curr_note && (!preset.note.quantize || (round(val + preset.note.jitter_threshold) != preset.note.curr_note && round(val - preset.note.jitter_threshold) != preset.note.curr_note))) {
          note_on(0, note, 127);
          note_off(0, preset.note.curr_note);
          preset.note.curr_note = note;
        }
        //Pitchbend
        if (!preset.note.quantize) {
          pitch_bend(0, 0.5 + (val - note) * 0.5);
        }
      }
      else if (preset.note.playing) {
        //Send note off
        note_off(0, preset.note.curr_note);
        preset.note.playing = false;
      }
      break;
  }

}