#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// AUDIO lib objects:

AudioSynthWaveform       someWaveform;      
AudioEffectEnvelope      envelope1;
//AudioOutputAnalog      audioOut;  // For the regular Teensy DAC
AudioOutputI2S           audioOut;  // For the Teensy Audio board over ISC
AudioConnection          patchCord0(someWaveform, envelope1);
AudioConnection          patchCord1(envelope1, 0, audioOut, 0);
AudioConnection          patchCord2(envelope1, 0, audioOut, 1);
AudioControlSGTL5000     audioBoard; //  For the Teensy Audio board over ISC

#define MAX_FREQ 600    // max frequency < - > arbitraryWaveform(array, maxFreq); 

/* include some wavetables: the files need to contain arrays [256] of type const short int wt1[] = { ... } */

#include "cello.h"
#include "hvoice.h"
#include "dbass.h"
#include "violin.h"



// the following presumably is needlessly complicated: 

struct WaveTable {
  int16_t const* table; 
};  

#define TABLE_NUM 4 // 

WaveTable waveTables[TABLE_NUM] {
  AKWF_cello_0001,
  AKWF_dbass_0001,
  AKWF_hvoice_0001,
  AKWF_violin_0001,
}; 

const uint16_t sequence[] = {65, 110, 85, 110, 55}; // a little sequence
#define SEQ_L 4       // sequence length 
#define SEQ_SPEED 200  // sequence speed (ms)
uint8_t waveselect=3, pos;  // variable to select wavetable, and position in sequence



void setup() {
  
  AudioMemory(12);
  Serial.begin(115200);
  audioBoard.enable();
  audioBoard.volume(0.45);
  
  /* // basic usage would be: 
  someWaveform.arbitraryWaveform(AKWF_violin_0001, MAX_FREQ); 
  someWaveform.begin(0.5, 440, WAVEFORM_ARBITRARY);
  */
}

void loop() {
 
 if (pos > SEQ_L) {
     pos = 0; // reset sequence
     waveselect = random(4);  // and pick a new wavetable
     const struct WaveTable* waveTable  = &waveTables[waveselect];
     int16_t* wave = const_cast<short int*>(waveTable->table);
     someWaveform.begin(0.5, 440, WAVEFORM_ARBITRARY);
     someWaveform.arbitraryWaveform(wave, MAX_FREQ);
          
     Serial.print("Using wavetable: ");
     Serial.println(waveselect);
 }
 
 // play next note in sequence
 someWaveform.frequency(sequence[pos++]);
 
 envelope1.noteOn();
 delay(SEQ_SPEED/2);
 envelope1.noteOff();
 delay(SEQ_SPEED);
 
}
