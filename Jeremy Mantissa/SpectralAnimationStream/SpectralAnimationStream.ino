
#include <Adafruit_NeoPixel.h>
#include "LinkedList.h"

#define PIN 5
#define LED_COUNT 150
#define SPECTRAL_ZONES 7
#define ZONE_SIZE 7

//values for MSGEQ7 chip
int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5; // reset is attached to digital pin 3
int spectrumValue[SPECTRAL_ZONES]; // to hold a2d values
int spectrumValuePrev[SPECTRAL_ZONES]; // to hold a2d values
float spectrumChange[SPECTRAL_ZONES];
//float spectrumMap[7];

// values for LEDs
//float ledID[SPECTRAL_ZONES];
//float ledSpeed[SPECTRAL_ZONES]; //?
uint32_t ledColor[SPECTRAL_ZONES]; // the color of each zone
//int stripColors[LED_COUNT*3]; // saved pixels in memory (for additive blend)
//float spectralAccum[SPECTRAL_ZONES];
//int accumInterval; // number of frames
float minBinAmplitude;
Node<uint32_t>* firstPixel;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel( LED_COUNT, PIN, NEO_GRB + NEO_KHZ800 );

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  
  minBinAmplitude = 20.0;
  
  firstPixel = create_node( strip.Color( 0, 0, 0) );
  Node<uint32_t>* lastPixel = firstPixel;
  
  for(int i=0; i<LED_COUNT-1; i++){
  
    Node<uint32_t>* p = create_node( strip.Color( 0, 0, 0) );
    insert_node_last( lastPixel, p ); 
    //lastPixel = p;
  }
  
  Serial.begin(9600);
  
  //display_list_serial(firstPixel);
  //Serial.println(get_list_size(firstPixel));
  
  
  // assign a unique color to each zone
  ledColor[0] = strip.Color(255, 0, 0);
  ledColor[1] = strip.Color(220, 75, 0);
  ledColor[2] = strip.Color(255, 255, 0);
  ledColor[3] = strip.Color(0, 255, 0);
  ledColor[4] = strip.Color(0, 255, 255);
  ledColor[5] = strip.Color(0, 0, 255);
  ledColor[6] = strip.Color(255, 0, 153);
  
  for(int i=0; i<SPECTRAL_ZONES; i++){
   
     spectrumValue[i] = 0;
     spectrumValuePrev[i] = 0;
     spectrumChange[i] = 0.0; 
  }
  
  /*
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  int spacing = LED_COUNT/SPECTRAL_ZONES;
  
  // create a random starting position and speed
  for(int i=0; i<SPECTRAL_ZONES; i++){
    ledID[i] = spacing * i;
    ledSpeed[i] = float(random(20, 80))/1000;  // 0.001 to .0150
  }
  
  
  
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
  
   */
  
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
  
  calculateEnergeticBin();
   
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // push all saved pixels to the LED strip:
  
  Node<uint32_t>* n = firstPixel;
  
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      
      strip.setPixelColor(i, n->value);
      if( n->next != 0 ){
         n = n->next;
      }  
   }
  
   strip.show();
   strip.setBrightness( 60 );
    
   delay(33);
}

void getAudioSpectrum(){
 
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW); 
  
  //get readings from chip
  for (int i = 0; i < 7; i++) {

    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // to allow the output to settle
    
    spectrumValuePrev[i] = spectrumValue[i];
    
    // some smoothing 50%
    spectrumValue[i] *= 0.75;
    spectrumValue[i] += 0.25 * analogRead(analogPin)/4;
    
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

void calculateLoudestBin(){
 
  int loudestBin = 0;
  float loudestVal = spectrumValue[loudestBin];
  
  for(int i=1; i<SPECTRAL_ZONES; i++){
    
     //Serial.print( spectrumValue[i] );
     //Serial.print( "\t" );
   
     if(  spectrumValue[i] > loudestVal ){
      
         loudestVal = spectrumValue[i];
         loudestBin = i; 
     }
  }
  
  for(int i=0; i<SPECTRAL_ZONES; i++){
    
     Serial.print( spectrumValue[i] );
     Serial.print( "\t" );
  }
  
   Serial.println();
  
  // if we go louder than the threshold, add a pixel with that color
  
  if( loudestVal > minBinAmplitude ){
    
        //Serial.print( loudestBin );
        //Serial.print( "\t" );
        //Serial.println( loudestVal );
    
        // remove the last
        remove_node_last(firstPixel);
   
        Node<uint32_t>* p = create_node( ledColor[loudestBin] );
        insert_node_first( firstPixel, p ); 
        //firstPixel = p;
  
  } else {
    
    // Serial.println( loudestBin );
  }
}

void calculateEnergeticBin(){
 
 
  float changes[SPECTRAL_ZONES];
  
  for(int i=0; i<SPECTRAL_ZONES; i++){
    
     float change = spectrumValue[i] - spectrumValuePrev[i];
     
     // relative changes
     changes[i] = change / spectrumValue[i];
  }

  int energeticBin = 0;
  float energeticVal = changes[energeticBin];
  
  for(int i=1; i<SPECTRAL_ZONES; i++){
    
     if( changes[i] > energeticVal ){
     
         energeticVal = changes[i];
         energeticBin = i;
     }
  }
  
  // if we go louder than the threshold, add a pixel with that color
  
  if( energeticVal > 0.075 ){
    
        //Serial.print( loudestBin );
        //Serial.print( "\t" );
        //Serial.println( loudestVal );
        
        float amp = float(spectrumValue[energeticBin]) / 255;
        
        int nPix = ( amp * 4 ) + 1;
        
        uint32_t c = ledColor[energeticBin];
        uint32_t r = (uint8_t)(c >> 16);
        uint32_t g = (uint8_t)(c >>  8);
        uint32_t b = (uint8_t)c;
          
        // mult by volume
        r *= amp;
        g *= amp;
        b *= amp;
    
        for(int i=0; i<nPix; i++){
    
            // remove the last
            remove_node_last(firstPixel);
     
            Node<uint32_t>* p = create_node( strip.Color( r, g, b) ); //ledColor[energeticBin] );
            insert_node_first( firstPixel, p ); 
        }
  
  } else {
    
     //remove_node_last(firstPixel);
    
     //Node<uint32_t>* p = create_node( strip.Color( 0, 0, 0) );
     //insert_node_first( firstPixel, p );
    // Serial.println( loudestBin );
  }
}

  
    


