#include <Wisp.h> /* Import Wisp class */
#include <FastLED.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define NUM_LEDS 29 /* LED's in the strip */
#define DATA_PIN 3 /* LED Data Pin */

#define WISP_1_HUE 0 /* Wisp 1 starting hue */
#define WISP_2_HUE 20 /* Wisp 2 starting hue */
#define WISP_1_START_POS 0 /* Wisp 1 starting position */
#define WISP_2_START_POS 15 /* Wisp 2 starting position */
#define WISP_HUE_INCREMENT 2 /* How much the color of the Wisps changes per loop */
#define BASE_TRAIL 13 /* Starting wisp trail length */
#define BASE_SPEED 100 /* Starting wisp rotation speed */
#define BASE_BRIGHTNESS_STEP 8 /* Starting brightness increment between wisp and trail elements */
#define BASE_HUE_STEP 3 /* Starting hue increment between wisp and trail elements */
#define BASE_BRIGHTNESS 100 /* Base Wisp brightness */
#define BRIGHTNESS_EASE_INCREMENT 5 /* How much Wisp brightness changes per step */

/* Teensy Audio library instantiations */
// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=239,495
AudioMixer4              mixer1;         //xy=466,557
AudioOutputI2S           i2s1;           //xy=668,405
AudioAnalyzePeak         peak1;          //xy=720,609
AudioAnalyzePeak         peak2;          //xy=737,682
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
AudioConnection          patchCord5(mixer1, peak1);
AudioConnection          patchCord6(mixer1, peak2);
AudioControlSGTL5000     sgtl5000_1;     //xy=543,296
// GUItool: end automatically generated code

uint8_t dataPin = DATA_PIN;    // Yellow wire on Adafruit Pixels
CRGB leds[NUM_LEDS]; /* Initialize FastLED leds array */

/* Instantiate Wisp objects (you can add more than 2) */
Wisp wisp1(WISP_1_START_POS, BASE_TRAIL, BASE_BRIGHTNESS, WISP_1_HUE);
Wisp wisp2(WISP_2_START_POS, BASE_TRAIL, BASE_BRIGHTNESS, WISP_2_HUE);

/* Variables to be changed in the loop */
int speed = BASE_SPEED; /* Speed of Wisps (as of now they share the speed globally) */
int brightness_step = BASE_BRIGHTNESS_STEP; /* How much brightness is lost from one trail pixel to the next */
int new_brightness; /* Temp variable for the new calculated brightness */

void setup() {
  /* Audio library stuffs */
  AudioMemory(16); /* Allocate audio memory */
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.3); /* Set sgtl5000 volume */
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5); /* Set mixer channel volumes (these send info to peaks) */
  SPI.setMOSI(7); /* Init SD card reading stuff */
  SPI.setSCK(14);

  /* Make sure SD card can be read */
  if (!(SD.begin(10))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  delay(1000); /* Necessary for card reading */

  /* Instantiate FastLED */
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();
}

void loop() {
  /* Clear previous pixels */
  FastLED.clear();

  /* Play wav file if it's not playing */
  if (playSdWav1.isPlaying() == false) {
    playSdWav1.play("PXLWV.WAV");
    delay(10); // wait for library to parse WAV info
  }

  /* Calculate the new speed based on the peak value (it's between 0.0 and 1.0) */
  speed = BASE_SPEED - (int)( peak1.read() * (BASE_SPEED * 0.9));
  if(speed < 0) speed = 0; /* In case speed drops below 0, we don't want that */

  /* Calculate new brightness, ease brightness toward the new value */
  new_brightness = BASE_BRIGHTNESS + (int)( peak2.read() * 100);
  if(wisp1.get_brightness() < new_brightness && wisp1.get_brightness() < (new_brightness - BRIGHTNESS_EASE_INCREMENT)) {
    wisp1.set_brightness(wisp1.get_brightness() + BRIGHTNESS_EASE_INCREMENT);
    wisp2.set_brightness(wisp2.get_brightness() + BRIGHTNESS_EASE_INCREMENT);
  } else if(wisp1.get_brightness() > new_brightness && wisp1.get_brightness() > (new_brightness + BRIGHTNESS_EASE_INCREMENT)) {
    wisp1.set_brightness(wisp1.get_brightness() - BRIGHTNESS_EASE_INCREMENT);
    wisp2.set_brightness(wisp2.get_brightness() - BRIGHTNESS_EASE_INCREMENT);
  } else {
    wisp1.set_brightness(new_brightness);
    wisp2.set_brightness(new_brightness);
  }

  /* Change the brightness step so taht brightness of trail pixels doens't go below 0 */
  brightness_step = new_brightness / (NUM_LEDS / 1.5);

  /* If Wisps reach the end of the strip, start over at the beginning */
  if(wisp1.get_pos() > (NUM_LEDS - 1)) wisp1.set_pos(0);
  if(wisp2.get_pos() > (NUM_LEDS - 1)) wisp2.set_pos(0);

  /* Shift Wisp hue over time, if it goes above 255, reset to 0 */
  wisp1.set_hue(wisp1.get_hue() + WISP_HUE_INCREMENT);
  wisp2.set_hue(wisp2.get_hue() + WISP_HUE_INCREMENT);
  if((wisp1.get_hue() + 2) > 255) wisp1.set_hue(0);
  if((wisp2.get_hue() + 2) > 255) wisp2.set_hue(0);

  /* Set the Wisp pixel color and brightness */
  leds[wisp1.get_pos()] = CHSV(wisp1.get_hue(), 180, wisp1.get_brightness());
  leds[wisp2.get_pos()] = CHSV(wisp2.get_hue(), 180, wisp2.get_brightness());

  /* Update Wisp positions and set trail pixel LED colors */
  wisp1.update(leds, brightness_step, NUM_LEDS, BASE_HUE_STEP);
  wisp2.update(leds, brightness_step, NUM_LEDS, BASE_HUE_STEP);

  /* Show show the new pixel values and delay the loop based on speed */
  FastLED.show(); 
  delay(speed);
}

