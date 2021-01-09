#include <Arduino.h>
#include "MidiUSB.h"

void setup() {
  Serial.begin(31250);
}

void noteOn(byte channel, byte note, byte velocity) {
  midiEventPacket_t event = {0x09, 0x90 | channel, note, velocity};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void noteOff(byte channel, byte note, byte velocity) {
  midiEventPacket_t event = {0x08, 0x80 | channel, note, velocity};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}

void loop() {
  byte channel = 0;
  noteOn(channel, 60, 127); //Note on
  delay(500);
  noteOff(channel, 60, 127); //Note on
  delay(500);
}