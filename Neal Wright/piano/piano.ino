// FFT Test
//
// Compute a 1024 point Fast Fourier Transform (spectrum analysis)
// on audio connected to the Left Line-In pin.  By changing code,
// a synthetic sine wave can be input instead.
//
// The first 40 (of 512) frequency analysis bins are printed to
// the Arduino Serial Monitor.  Viewing the raw data can help you
// understand how the FFT works and what results to expect when
// using the data to control LEDs, motors, or other fun things!
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6
#define COLUMNS 40
#define ROWS 15
#define MATRIX_WIDTH 40
#define MATRIX_HEIGHT 1
#define TILE_WIDTH 1
#define TILE_HEIGHT 15

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(40, 1, 1, 15, PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_ZIGZAG +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);

const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//
AudioInputI2S          audioInput;         // audio shield: mic or line-in
AudioSynthWaveformSine sinewave;
AudioAnalyzeFFT1024    myFFT;
AudioOutputI2S         audioOutput;        // audio shield: headphones & line-out

// Connect either the live input or synthesized sine wave
AudioConnection patchCord1(audioInput, 0, myFFT, 0);
//AudioConnection patchCord1(sinewave, 0, myFFT, 0);

AudioControlSGTL5000 audioShield;

  float history[(ROWS - 1)][COLUMNS];

void setup() {

  Serial.begin(9600);

  matrix.begin();
  matrix.show(); // Initialize all pixels to 'off'
  matrix.setBrightness(60);
  
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(0.5);

  // Configure the window algorithm to use
  myFFT.windowFunction(AudioWindowHanning1024);
  //myFFT.windowFunction(NULL);

//  // Create a synthetic sine wave, for testing
//  // To use this, edit the connections above
//  sinewave.amplitude(0.8);
//  sinewave.frequency(1034.007);

  for(int i = 0; i < (ROWS - 1); i++) {
    history[i][0] = 0;
  }
  

}

void loop() {
  float bands[40];
  int color_vals[3];

  if (myFFT.available()) {

    bands[0] =  myFFT.read(0);
    bands[1] =  myFFT.read(1);
    bands[2] =  myFFT.read(2, 3);
    bands[3] =  myFFT.read(4, 5);
    bands[4] =  myFFT.read(6, 8);
    bands[5] =  myFFT.read(9, 11);
    bands[6] =  myFFT.read(12, 14);
    bands[7] =  myFFT.read(15, 18);
    bands[8] =  myFFT.read(19, 23);
    bands[9] =  myFFT.read(24, 28);
    bands[10] = myFFT.read(29, 34);
    bands[11] = myFFT.read(35, 40);
    bands[12] = myFFT.read(41, 48);
    bands[13] = myFFT.read(49, 58);
    bands[14] = myFFT.read(59, 69);
    bands[15] = myFFT.read(70, 80);
    bands[16] = myFFT.read(81, 91);
    bands[17] = myFFT.read(92, 102);
    bands[18] = myFFT.read(103, 114);
    bands[19] = myFFT.read(115, 126);
    bands[20] = myFFT.read(127, 139);
    bands[21] = myFFT.read(140, 154);
    bands[22] = myFFT.read(155, 170);
    bands[23] = myFFT.read(171, 187);
    bands[24] = myFFT.read(188, 204);
    bands[25] = myFFT.read(205, 225);
    bands[26] = myFFT.read(226, 246);
    bands[27] = myFFT.read(247, 269);
    bands[28] = myFFT.read(270, 293);
    bands[29] = myFFT.read(294, 318);
    bands[30] = myFFT.read(319, 344);
    bands[31] = myFFT.read(345, 369);
    bands[32] = myFFT.read(370, 390);
    bands[33] = myFFT.read(391, 415);
    bands[34] = myFFT.read(416, 438);
    bands[35] = myFFT.read(439, 455);
    bands[36] = myFFT.read(456, 470);
    bands[37] = myFFT.read(471, 490);
    bands[38] = myFFT.read(491, 500);
    bands[39] = myFFT.read(500, 511);

    set_pixels(bands, history, color_vals);
    matrix.show();
  }
}

void set_pixels(float bands[], float history[][COLUMNS], int color_vals[]) {
  // So many loop variables...
  int i, j, k, l, m;

  // Loop through the columns and get the colors
  for(i = 0; i < COLUMNS; i++) {

    // Get the color based on the amplitude
    get_amp_color(bands[i], color_vals);
    // Set initial row colors
    matrix.drawPixel( 15, i ,drawRGB24toRGB565(color_vals[0], color_vals[1], color_vals[2]));

    // Set any populated history LED's
    for(j = (ROWS - 2); j >= 0; j--) {

      // Only show history LED's if history has been set
      if(history[j][0] != 0) {

        // Set history LED's
        matrix.drawPixel( j, i ,drawRGB24toRGB565(color_vals[0], color_vals[1], color_vals[2]));
      }
    }
  }

  for(l = 0; l < COLUMNS; l++) {
    // Set last row of history to current row
    history[(ROWS - 2)][l] = bands[l];
  }
  
  for(k = (ROWS - 2); k >= 0; k--) {
    if(history[k][0] != 0 && k != 0) {
      for(m = 0; m < COLUMNS; m++) {
        history[(k - 1)][m] = history[k][m];
      }
    }
  }
  matrix.drawPixel(1, 2, drawRGB24toRGB565( 255, 0, 0));
}

void get_amp_color(float amplitude, int color_vals[]) {
  float r, g, b;
  float min = 0.05;
  float max = 0.70;
  float steps[6];
  float step_number = 6.0;
  float step = (max - min) / step_number;
  int i;

  for(i = 0; i < step_number; i++) {
    steps[i] = step * i;
  }

  if(amplitude > min && amplitude <= steps[1]) {
    r = (amplitude - steps[1]) / step;
    g = 0;
    b = 1;
  } else if(amplitude > steps[1] && amplitude <= steps[2]) {
    r = 0;
    g = (amplitude - steps[2]) / step;
    b = 1;
  } else if(amplitude > steps[2] && amplitude <= steps[3]) {
    r = 0;
    g = 1;
    b = (amplitude - steps[3]) / step;
  } else if(amplitude > steps[3] && amplitude <= steps[4]) {
    r = (amplitude - steps[4]) / step;
    g = 1;
    b = 0;
  } else if(amplitude > steps[4] && amplitude <= steps[5]) {
    r = 1;
    g = (amplitude - steps[5]) / step;
    b = 0;
  } else if(amplitude > steps[5] && amplitude <= max) {
    r = 1;
    g = 0;
    b = 0;
  } else if(amplitude <= min) {
    Serial.println("true");
    r = 0;
    g = 0;
    b = 0;
  }
  
  color_vals[0] = r * 255;
  color_vals[1] = g * 255;
  color_vals[2] = b * 255;

}

uint16_t drawRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}
