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
#define ROWS 14
#define MATRIX_WIDTH 40
#define MATRIX_HEIGHT 1
#define TILE_WIDTH 1
#define TILE_HEIGHT 15
#define mBrightness 255//matrix brightness


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(40, 14, 1, 1, PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
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

  // History array for propagation
  float history[ROWS - 1][COLUMNS];
  int prop_speed = 0;
  int prop_target = 0;
  int max_prop_speed = 10;
  int count = 0;

  //for standby playme
  float timer = 0;
  float fade = 0.00;
  boolean fade_direction = true;
  boolean touched = false;
  float fadeholdtime = 0;
  boolean fading = true;
  
void setup()
{

  Serial.begin(9600);

  matrix.begin();
  matrix.show(); // Initialize all pixels to 'off'
  matrix.setBrightness(mBrightness);
  matrix.setTextColor(drawRGB24toRGB565(0, 0, 0));
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

  // Set all history values for -1 to show that they're empty (there is no such thing as an "empty" array index in C++)
  for(int y = 0; y < (ROWS - 1); y++) {
    for(int x = 0; x < COLUMNS; x++) {
      history[y][x] = -1;
    }
  }

}

void loop()
{

  // Set arrays for the current row and the RGB vals
  float bands[40];
  float sum = 0;
  int color_vals[3];

  //for standby playme
  float amp_threshold = 0.10;
  int timeout = 700;

  // Read in FFT values
  if (myFFT.available()) {

    // Set sampling ranges based on function
    for (int k=0; k<40; k++){
      if (k >=28){
        float amp = 0.05 * pow((k - 5), 2);
        bands[k] = myFFT.read(amp, amp + 5);//note for tim after middle good work on begginging
        sum += bands[k];
      }else {
        float amp = 0.9 * pow((k), 0.035 * k);
        bands[k] = myFFT.read(amp, amp + 5);//note for tim after middle good work on begginging
        sum += bands[k];
      }
    }

    /* Start standby text */

    if(timer == 0) {
      timer = millis();
      fade = 0.00;
    }
    
    if ((millis() - timer) > timeout && sum < amp_threshold){
      matrix.setBrightness(255);
      matrix.setCursor(2, 4);
      matrix.setTextColor(drawRGB24toRGB565((fade * 255), (fade * 255), (fade * 255)));
      matrix.setTextSize(1);
      matrix.setTextWrap(false);
      matrix.print("Play");
      matrix.setCursor(27, 4);
      matrix.print("Me");
      matrix.show();
      touched = false;
      
      if(fade >= 0.98) {
        fade_direction = false;
      }
      if (fade <= 0.7){
        if (fade_direction == false && fading == true){
          fadeholdtime = millis();
          fading = false;
        }else if (millis()- fadeholdtime >= 4000){
          fade_direction = true;
          fading = true;
        }
      }
      if(fade < 0.98 && fade_direction && fading) {
        fade += 0.02;
      }
      if (fade > 0.02 && !fade_direction  && fading) {
        fade -= 0.015;
      }

    } else if (sum >=  amp_threshold){ //because it always goes here
      // Set all of the necessary pixels and display them
      set_pixels(bands, history, color_vals);
      matrix.setBrightness(mBrightness);
      timer = millis();
      matrix.show();
      if(!touched) {
        timer = millis();
        fade = 0.00;
        touched = true;
      }
    } else{ // to finish the animation
      set_pixels(bands, history, color_vals);
      matrix.setBrightness(mBrightness);
      matrix.show();
    }
  }

  count += 0.1;
}


void set_pixels(float bands[], float history[][COLUMNS], int color_vals[])
{
  // Set a loop variable for x (column) and y (row)
  int x, y;

  // Loop through the columns and get the colors for the bottom row (AKA row 13)
  for(x = 0; x < COLUMNS; x++) {

    // Get the color based on the amp
    get_amp_color(bands[x], color_vals);
    // Set initial row colors
    matrix.drawPixel( x, 13 ,drawRGB24toRGB565(color_vals[0], color_vals[1], color_vals[2]));

  }

  // Loop through the rest of the columns and set the colors if the row has been populated
  for(y = ROWS - 2; y >= 0; y--) {
    for(x = 0; x < COLUMNS; x++) {
      if(history[y][x] != -1) {
        get_amp_color(history[y][x], color_vals);
        matrix.drawPixel( x, y ,drawRGB24toRGB565(color_vals[0], color_vals[1], color_vals[2]));
      }
    }
  }

  // Move the rows up the history chain
  for(y = 0; y < ROWS - 2; y++) {
    for(x = 0; x < COLUMNS; x++) {
      if(history[y + 1][x] != -1 && y != 12) {
        history[y][x] = history [y + 1][x];
      }
    }
  }

  // Set the very next row equal to the lowest row
  for(x = 0; x < COLUMNS; x++) {
    history[12][x] = bands[x];
  }
  
}

void get_amp_color(float amp, int color_vals[])
{
  float r, g, b;
  float min = 0.01; // Minimum amplitude that triggers color
  float max = 0.20; // Maximum aplitude (red)
  float steps[6];
  float step_number = 6.0; // Number of steps to between minimum and maximum
  float step = (max - min) / step_number; // Calculate the step value
  int i;

  r = 0;
  g = 0;
  b = 0;

  for(i = 0; i < step_number; i++) {
    steps[i] = step * i;
  }

  // For each step, giv
  if(amp > min && amp <= steps[1]) {
    r = (amp - steps[1]) / step;
    g = 0;
    b = 1;
  } else if(amp > steps[1] && amp <= steps[2]) {
    r = 0;
    g = (amp - steps[2]) / step;
    b = 1;
  } else if(amp > steps[2] && amp <= steps[3]) {
    r = 0;
    g = 1;
    b = (amp - steps[3]) / step;
  } else if(amp > steps[3] && amp <= steps[4]) {
    r = (amp - steps[4]) / step;
    g = 1;
    b = 0;
  } else if(amp > steps[4] && amp <= steps[5]) {
    r = 1;
    g = (amp - steps[5]) / step;
    b = 0;
  } else if(amp > steps[5] && amp <= max) {
    r = 1;
    g = 0;
    b = 0;
  } else if(amp <= min) {
    r = 0;
    g = 0;
    b = 0;
  } else if(amp > max) {
    r = 1;
    g = 0;
    b = 0;
  }
  
  color_vals[0] = r * 255;
  color_vals[1] = g * 255;
  color_vals[2] = b * 255;

}

// Convert to 16 bit RGB value (565) from 3 8 bit R, G, and B values
uint16_t drawRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}
