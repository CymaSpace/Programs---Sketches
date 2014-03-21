/* LED "Color Organ" for Adafruit Trinket and NeoPixel LEDs.

Hardware requirements:
 - Adafruit Trinket or Gemma mini microcontroller (ATTiny85).
 - Adafruit Electret Microphone Amplifier (ID: 1063)
 - Several Neopixels, you can mix and match
   o Adafruit Flora RGB Smart Pixels (ID: 1260)
   o Adafruit NeoPixel Digital LED strip (ID: 1138)
   o Adafruit Neopixel Ring (ID: 1463)

Software requirements:
 - Adafruit NeoPixel library

Connections:
 - 5 V to mic amp +
 - GND to mic amp -
 - Analog pinto microphone output (configurable below)
 - Digital pin to LED data input (configurable below)

Written by Adafruit Industries.  Distributed under the BSD license.
This paragraph must be included in any redistribution.
*/
#include <Adafruit_NeoPixel.h>

#define N_PIXELS  60  // Number of pixels you are using
#define MIC_PIN    1  // Microphone is attached to Trinket GPIO #2/Gemma D2 (A1)
#define LED_PIN    0  // NeoPixel LED strand is connected to GPIO #0 / D0
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     50  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS +1) // Allow dot to go slightly off scale
// Comment out the next line if you do not want brightness control or have a Gemma
#define POT_PIN    3  // if defined, a potentiometer is on GPIO #3 (A3, Trinket only) 

byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
  
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,     // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  memset(vol, 0, sizeof(vol));
  strip.begin();
}
void loop() {
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;
  n   = analogRead(MIC_PIN);                 // Raw reading from mic 
  n   = abs(n - 512 - DC_OFFSET);            // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);      // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
  
  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if(height < 0L)       height = 0;      // Clip output
  else if(height > TOP) height = TOP;
  if(height > peak)     peak   = height; // Keep 'peak' dot at top

// if POT_PIN is defined, we have a potentiometer on GPIO #3 on a Trinket 
//    (Gemma doesn't have this pin)
  uint8_t bright = 255;   
#ifdef POT_PIN 
   bright = analogRead(POT_PIN);  // Read pin (0-255) (adjust potentiometer 
                                  //   to give 0 to Vcc volts
#endif
  strip.setBrightness(50);    // Set LED brightness (if POT_PIN at top
                                  //  define commented out, will be full)
  // Color pixels based on rainbow gradient
  for(i=0; i<N_PIXELS; i++) {  
    if(i >= height)               
       strip.setPixelColor(i,   0,   0, 0);
    else 
       strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    } 

   strip.show(); // Update strip

  vol[volCount] = n;                      // Save sample for dynamic leveling
  if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for(i=1; i<SAMPLES; i++) {
    if(vol[i] < minLvl)      minLvl = vol[i];
    else if(vol[i] > maxLvl) maxLvl = vol[i];
  }
  // minLvl and maxLvl indicate the volume range over prior frames, used
  // for vertically scaling the output graph (so it looks interesting
  // regardless of volume level).  If they're too close together though
  // (e.g. at very low volume levels) the graph becomes super coarse
  // and 'jumpy'...so keep some minimum distance between them (this
  // also lets the graph go to zero when no sound is playing):
  if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
