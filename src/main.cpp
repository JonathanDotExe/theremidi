#include <Arduino.h>
#include "MIDIUSB.h"

void setup() {
  
}

void loop() {
  byte channel = 0;
  MidiUSB.sendMIDI({0x09, 0x90 | channel, 60, 127}); //Note on
  delay(500);
  MidiUSB.sendMIDI({0x08, 0x80 | channel, 60, 127}); //Note off
  delay(500);
}