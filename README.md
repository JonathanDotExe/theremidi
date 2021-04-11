# theremidi
Software for a MIDI Theremin built using an ESP32

# Deutsche Beschreibung
Theremidi ist eins ESP32 - Programm, welches den ESP in einen Theremin-artigen MIDI Controller verwandelt. Am ESP32 wird dazu
ein Adafruit VL53L0X - Laserabstandssensor angeschlossen. Basierend auf diesen Messwerten können durch Bewegung der Hand,
dann verschiedene MIDI-Parameter gesteuert werden. Theremidi unterstützt zurzeit einen CC - Modus mit dem man einen Controller - Wert
steuern kann (z.B. Modulationen wie Vibrato oder Filter-Cutoff) und einen Pitch-Bend-Modus mit dem man dem Pitch-Bend Regler
steuern kann (um die Tonhöhe der gespielten Noten zu verändern).

Die MIDI - Daten werden über den USB-to-Serial Port an den Computer gesendet und von dem Program "The Hairless MIDI <-> Serial Bridge"
(https://projectgus.github.io/hairless-midiserial/) als MIDI an einen MIDI-Ausgang gesendet. So kann dann ein Hardware oder Software
Synthesizer gesteuert werden.

# Used Resources
* http://www.esp32learning.com/code/vl53l0x-time-of-flight-sensor-and-esp32.php
* https://projectgus.github.io/hairless-midiserial/
* https://www.arduino.cc/en/Tutorial/BuiltInExamples/Midi

# Used libraries
* Adafruit VL53L0X library licensed under MIT (https://github.com/adafruit/Adafruit_VL53L0X)
