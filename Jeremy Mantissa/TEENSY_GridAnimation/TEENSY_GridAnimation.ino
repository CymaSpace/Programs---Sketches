
#include <Adafruit_NeoPixel.h>

#define PINA 21
#define PINB 22
#define PINC 23
#define LED_COUNT 10
#define SPECTRAL_ZONES 6
#define GRID_W 18
#define GRID_H 4
#define GRID_D 3

int fftColors[SPECTRAL_ZONES];
uint32_t ledGridXZ[GRID_W][GRID_D];
uint32_t ledGridXY[GRID_W][GRID_H];
int stripAMap[GRID_W];
int stripBMap[GRID_W];
int stripCMap[GRID_W];
int washOffset = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel stripA = Adafruit_NeoPixel(10 * GRID_H, PINA, NEO_GRB + NEO_KHZ800); // back
Adafruit_NeoPixel stripB = Adafruit_NeoPixel(10 * GRID_H, PINB, NEO_GRB + NEO_KHZ800); // mid 
Adafruit_NeoPixel stripC = Adafruit_NeoPixel(10 * GRID_H, PINC, NEO_GRB + NEO_KHZ800); // front

void setup() {
  
  stripA.begin();
  stripA.show(); // Initialize all pixels to 'off'
  
  stripB.begin();
  stripB.show(); // Initialize all pixels to 'off'
  
  stripC.begin();
  stripC.show(); // Initialize all pixels to 'off'

  // our color palette
  fftColors[0] = stripA.Color(255, 0, 0);
  fftColors[1] = stripA.Color(0, 255, 0);
  fftColors[2] = stripA.Color(0, 0, 255);
  fftColors[3] = stripA.Color(255, 255, 0);
  fftColors[4] = stripA.Color(0, 255, 255);
  fftColors[5] = stripA.Color(255, 0, 255);
  
  // rear strip
  stripCMap[0] = 0;
  stripCMap[1] = 2;
  stripCMap[2] = 4;
  stripCMap[3] = 6;
  stripCMap[4] = 8;
  stripCMap[5] = 9;
  stripCMap[6] = 11;
  stripCMap[7] = 13;
  stripCMap[8] = 15;
  stripCMap[9] = 17;
  
  // mid strip
  stripBMap[0] = 1;
  stripBMap[1] = 3;
  stripBMap[2] = 5;
  stripBMap[3] = 7;
  stripBMap[4] = 10;
  stripBMap[5] = 12;
  stripBMap[6] = 14;
  stripBMap[7] = 16;
  stripBMap[8] = -1;
  stripBMap[9] = -1;

  // front strip
  stripAMap[0] = 2;
  stripAMap[1] = 4;
  stripAMap[2] = 6;
  stripAMap[3] = 11;
  stripAMap[4] = 13;
  stripAMap[5] = 15;
  stripAMap[6] = -1;
  stripAMap[7] = -1;
  stripAMap[8] = -1;
  stripAMap[9] = -1;
}

void loop() {
  
  // apply colors accorss the grid
  
   for(int i=0; i<GRID_W; i++){
   
      int ledID = (i + washOffset) % SPECTRAL_ZONES;
      ledGridXZ[i][0] = fftColors[ ledID ];
      //ledID = (i + washOffset +1) % SPECTRAL_ZONES;
      ledGridXZ[i][1] = fftColors[ ledID ];
      //ledID = (i + washOffset+2) % SPECTRAL_ZONES;
      ledGridXZ[i][2] = fftColors[ ledID ];
  }
  
  // apply the grid pixels to the LEDs
  // it's just like an image/bitmap applied to a 3D grid
  
  for(uint16_t i=0; i<stripC.numPixels()/GRID_H; i++){
      for(uint16_t j=0; j<GRID_H; j++){
      
        int p = i * GRID_H + j;
        
        // make sure LED exits on strip 
        // read position from map
        // then apply color from grid to LED
        if( stripAMap[i] != -1 ) stripA.setPixelColor(p, ledGridXZ[stripAMap[i]][0]);
        if( stripBMap[i] != -1 ) stripB.setPixelColor(p, ledGridXZ[stripBMap[i]][1]);
        if( stripCMap[i] != -1 ) stripC.setPixelColor(p, ledGridXZ[stripCMap[i]][2]);
      }
  }
  
  // dim and then show the LEDz
  
   stripA.setBrightness(120);
   stripB.setBrightness(120);
   stripC.setBrightness(120);
   stripA.show();
   stripB.show();
   stripC.show();
   
   // increment the color offset
   // cycle when over the number of LEDs in the grid (horizontally)
   
   washOffset++;
   
   if( washOffset >= GRID_W ){
    
       washOffset = 0; 
   }
   
   delay(300);
  
  /*
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
  */
}

// clear pixel memory
void clearPixels(){
   for(uint16_t i=0; i<stripA.numPixels(); i++) {
      //stripColors[i] = stripA.Color(0, 0, 0);
  }
}

void applyPixels(){
    for(uint16_t i=0; i<stripA.numPixels(); i++) {
      //strip.setPixelColor(i, stripColors[i]);
    }
  
  stripA.show();
}

/*

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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

*/

