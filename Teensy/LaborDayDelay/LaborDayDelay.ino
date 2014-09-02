/*
Some fun madness with the delay object and samples from the AudioFromSketch example.
Requires 3 pots & a button, plus an optional pot for volume.
Requires Teensy 3.1 and the Teensy audio board

YouTube: http://youtu.be/Cu9Scg-ZeGE
Github: https://github.com/mihow/audio-patches/Teensy

by Mihow / Michael Bunsen
*/

#include <Audio.h>
#include <Wire.h>
#include <SD.h>

#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleCashRegister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

AudioPlayMemory  kick;
AudioPlayMemory  cashregister;
AudioSynthWaveformSine sine1;
AudioEffectMultiply sinekick;
AudioEffectDelay delay1;
AudioMixer4 delaydampen;
AudioMixer4 mixdown;
AudioMixer4 delaymix;
AudioOutputI2S audioOutput;


// Main samples, sent to mixer
AudioConnection patchCord10(kick, 0, mixdown, 0);
AudioConnection patchCord20(cashregister, 0, mixdown, 2);

// Multiplying a sine with the kick for some cordinated higher pitches
AudioConnection patchCord30(sine1, 0, sinekick, 0);
AudioConnection patchCord40(kick, 0, sinekick, 1);
AudioConnection patchCord50(sinekick, 0, mixdown, 1);

// Send the main mix to delay.
AudioConnection patchCord60(mixdown, 0, delay1, 0);
// Send the delay to it's own mixer so we can change the
// gain of each channel (to taper off the volume). 
AudioConnection patchCord70(delay1, 0, delaymix, 0);
AudioConnection patchCord80(delay1, 1, delaymix, 1);
AudioConnection patchCord90(delay1, 2, delaymix, 2);
AudioConnection patchCord100(delay1, 3, delaymix, 3);
// Mixdown the delay with another mixer so we can taper off
// the volume of the whole thing before feeding back to the 
// delay again.
AudioConnection patchCord110(delaymix, 0, delaydampen, 0);
AudioConnection patchCord120(delaydampen, 0, mixdown, 3);

// Send the final mix to our output
AudioConnection patchCord130(mixdown, 0, audioOutput, 0);
AudioConnection patchCord140(mixdown, 0, audioOutput, 1);

// Using the Teensy audio shield over ISC
AudioControlSGTL5000 audioShield;


void setup() {
  
  audioShield.enable();
  audioShield.volume(0);
  
  // The highest value before I can't compile
  AudioMemory(216);

  sine1.amplitude(0.3);

  // Taper off the delay so later ones are quieter.
  delaymix.gain(0, 0.8);
  delaymix.gain(1, 0.6);
  delaymix.gain(2, 0.4);
  delaymix.gain(3, 0.2);
  
  // The gain of the entire delay output before feeding back
  // Changes to this value have a big + fun effect. 
  delaydampen.gain(0, 0.8);
  
}


void loop() {
  // I can't access the digital pins due to how my Teensy +
  // Audio board sit on my breadboard. So this is a button
  // wired to an analog input. Haha.
  int buttonish = analogRead(21);
  int knob1 = analogRead(15); // Volume knob
  int knob2 = analogRead(16); // Time scaling between delays
  int knob3 = analogRead(17); // Main delay time
  int knob4 = analogRead(20); // Time between drum hits
  
  // Scale the values in whatever way sounds good & have
  // a lot of good effects from top to bottom. 
  // The pots I have wired are non-linear, which makes
  // the adjustments a bit strange. 
  // I didn't test any of these in the serial console either.
  float n1 = (float)knob1 / 1024; 
  float n2 = (float)knob2 / 2048;
  float n3 = (float)knob3 / 14;
  float n4 = (float)knob4 / 1.8;


  audioShield.volume(n1);
  
  sine1.frequency(512 - n4);
  
  // Here is the delay timing
  // Knob 3 is the main delay time
  // Knob 2 increases the "scaled time" between delays
  // Haven't done any memory logging, etc. but the effect is cool.
  delay1.delay(0, n3);
  delay1.delay(1, n3 * n2 * 1.2);
  delay1.delay(2, n3 * n2 * 1.4);
  delay1.delay(3, n3 * n2 * 1.5);
  
  if (buttonish < 10) {
    mixdown.gain(0, .2);
    cashregister.play(AudioSampleCashRegister);
    delay(200);
    mixdown.gain(0, 1.0);
  }

  // Overlapping madness/goodness when the tempo is fast
  kick.play(AudioSampleKick);

  delay(n4);
  
}

