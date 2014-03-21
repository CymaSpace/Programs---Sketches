
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define LED_COUNT 30
#define SPECTRAL_ZONES 7

float ledID[SPECTRAL_ZONES];
float ledSpeed[SPECTRAL_ZONES]; 
uint32_t ledColor[SPECTRAL_ZONES]; // the color of each zone
int stripColors[LED_COUNT*3]; // saved pixels in memory (for additive blend)

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // create a random starting position and speed
  for(int i=0; i<SPECTRAL_ZONES; i++){
    ledID[i] = random(LED_COUNT);
    ledSpeed[i] = float(random(20, 80))/1000;  // 0.001 to .0150
  }
  
  // assign a unique color to each zone
  ledColor[0] = strip.Color(255, 0, 0);
  ledColor[1] = strip.Color(0, 255, 0);
  ledColor[2] = strip.Color(0, 0, 255);
  ledColor[3] = strip.Color(127, 255, 0);
  ledColor[4] = strip.Color(0, 255, 255);
  ledColor[5] = strip.Color(255, 0, 127);
  ledColor[6] = strip.Color(0, 255, 127);
  
  for(int i=0; i<LED_COUNT*3; i++){
   
     stripColors[i] = 0; 
  }
}

void loop() {
  
  //strip.show();
  
  clearPixels();
  
  for(int i=0; i<SPECTRAL_ZONES; i++){
  
    for(uint16_t j=0; j<6; j++){
        
        int p = (int(ledID[i])+j)*3;
        
        // adjust the brightness (create trails)
        // first is brightest, last is still visible
        float br = float(7 - j) / 7;
        
        if( p < LED_COUNT*3 ){
        
          // get the zone color (stored as 32-bit int)
          uint32_t c = ledColor[i];
          
          // break into RGB components
          uint32_t r = (uint8_t)(c >> 16);
          uint32_t g = (uint8_t)(c >>  8);
          uint32_t b = (uint8_t)c;
          
          // add to pixel array (also 32-bit int)
          stripColors[p] += r * br;
          stripColors[p+1] += g * br;
          stripColors[p+2] += b * br;
        }
    }
    
    // increment the speed
    ledID[i] += ledSpeed[i];
    
    // if the position goes beyond 30, start @ zero
    if( ledID[i] >= LED_COUNT ){
       ledID[i] = 0; 
    }
  }
  
  applyPixels();
    
  delay(1);
}


void clearPixels(){
  
  // clear pixel memory:
  
  int totalPix = LED_COUNT * 3;
  
  // turn all saved pixels off
  for(uint16_t i=0; i<totalPix; i++) {
      stripColors[i] = 0; 
  }
}

void applyPixels(){
  
    // push all saved pixels to the LED strip:
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      
      // turn RGB to 32-bit int
      int p = i * 3;
      uint32_t c = strip.Color( min(stripColors[p], 255), min(stripColors[p+1], 255), min(stripColors[p+2], 255));
      strip.setPixelColor(i, c);
    }
  
    strip.show();
}

