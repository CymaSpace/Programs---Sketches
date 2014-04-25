
#include <Adafruit_NeoPixel.h>

#define PIN 5
#define LED_COUNT 150
#define SPECTRAL_ZONES 7
#define ZONE_SIZE 7

//values for MSGEQ7 chip
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values
float spectrumMap[7];

// values for LEDs
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
  
  Serial.begin(9600);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  int spacing = LED_COUNT/SPECTRAL_ZONES;
  
  // create a random starting position and speed
  for(int i=0; i<SPECTRAL_ZONES; i++){
    ledID[i] = spacing * i;
    ledSpeed[i] = float(random(20, 80))/1000;  // 0.001 to .0150
  }
  
  // assign a unique color to each zone
  ledColor[0] = strip.Color(255, 0, 0);
  ledColor[1] = strip.Color(220, 75, 0);
  ledColor[2] = strip.Color(255, 255, 0);
  ledColor[3] = strip.Color(0, 255, 0);
  ledColor[4] = strip.Color(0, 255, 255);
  ledColor[5] = strip.Color(0, 0, 255);
  ledColor[6] = strip.Color(255, 0, 153);
  
  for(int i=0; i<LED_COUNT*3; i++){
   
     stripColors[i] = 0; 
  }
  
  spectrumMap[0] = 4.0;
  spectrumMap[1] = 1.0;
  spectrumMap[2] = 1.0;
  spectrumMap[3] = 1.0;
  spectrumMap[4] = 1.0;
  spectrumMap[5] = 1.0;
  spectrumMap[6] = 1.0;
  spectrumMap[7] = 1.0;
  
  // spectrum shield
  pinMode(analogPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(DEFAULT);
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
  
}

void loop() {
  
  getAudioSpectrum();
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  clearPixels();
  
  for(int i=0; i<SPECTRAL_ZONES; i++){
    
    // max band length
    int stripLength = 12;
    
    int zoneW = min(float(spectrumValue[i])/255 * stripLength+1, stripLength+1);
  
    for(int j=0; j<zoneW; j++){
        
        int p = (int(ledID[i])+j)*3;
        
        // adjust the brightness (create trails)
        // first is brightest, last is still visible
        float br = float(zoneW+1-j)/zoneW; //float(ZONE_SIZE + 1 - j) / float(ZONE_SIZE);
        //br *= br;
        //br *= br;
        //br *= .75;
        
        if( p < LED_COUNT*3 ){
        
          // get the zone color (stored as 32-bit int)
          uint32_t c = ledColor[i];
          
          // break into RGB components
          uint32_t r = (uint8_t)(c >> 16);
          uint32_t g = (uint8_t)(c >>  8);
          uint32_t b = (uint8_t)c;
          
          // add to pixel array (also 32-bit int)
          stripColors[p] += br * r;
          stripColors[p+1] += br * g;
          stripColors[p+2] += br * b;
        }
        
        if( j != 0 ){
        
        p = (int(ledID[i])-j)*3;
        
        if( p >= 0 ){
         
            // get the zone color (stored as 32-bit int)
          uint32_t c = ledColor[i];
          
          // break into RGB components
          uint32_t r = (uint8_t)(c >> 16);
          uint32_t g = (uint8_t)(c >>  8);
          uint32_t b = (uint8_t)c;
          
          // add to pixel array (also 32-bit int)
          stripColors[p] += br * r;
          stripColors[p+1] += br * g;
          stripColors[p+2] += br * b;
        }
        }
    }
    
    
    // increment the speed
    ledID[i] += float(spectrumValue[i])/255; //ledSpeed[i];
    
    // if the position goes beyond 30, start @ zero
    if( ledID[i] >= LED_COUNT ){
       ledID[i] = 0; 
    } 
  }
  
  applyPixels();
    
  delay(1);
}

void getAudioSpectrum(){
 
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW); 
  
  //get readings from chip
  for (int i = 0; i < 7; i++) {

    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    
    // some smoothing 50%
    spectrumValue[i] *= 0.75;
    spectrumValue[i] += 0.25 * spectrumMap[i] * analogRead(analogPin)/4;
    
    // low pass filter
    //if( spectrumValue[i] < 33 ) spectrumValue[i] = 0;
    
    digitalWrite(strobePin, HIGH);
  }
  
  /*
  for(int i=0; i<7; i++){
   
      Serial.print(  spectrumValue[i]/4 );
      Serial.print(  '\t' );
  }
  Serial.println();
 */
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
    
    /*
    for(uint16_t i=0; i<7; i++) {
    
          int p = i * 3;
      
          Serial.print( i );
          Serial.print( '\t' );
          Serial.print( stripColors[p] );
          Serial.print( '\t' );
          Serial.print( stripColors[p+1] );
          Serial.print( '\t' );
          Serial.println( stripColors[p+2] );      
    }
   
   */
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      
      // turn RGB to 32-bit int
      int p = i * 3;
      uint32_t c = strip.Color( min(stripColors[p], 255), min(stripColors[p+1], 255), min(stripColors[p+2], 255));
      strip.setPixelColor(i, c);
    }
  
    strip.show();
    strip.setBrightness( 60 );
}

