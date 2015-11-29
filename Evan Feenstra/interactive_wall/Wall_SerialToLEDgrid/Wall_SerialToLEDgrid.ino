#include <FastLED.h>

#define NUM_LEDS 251 // Number of LED's in the strip
#define ROWS 10 //# of leds in each row (in vertical pos)
#define COLUMNS 25 //# of leds in each column

/* Output pin definitions */
#define DATA_PIN 6 // Data out
#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define SENSITIVITY_POT_PIN 3 // Right pot
#define STOMP_PIN 5 // The pin connected to the STOMP_PIN button
#define STROBE_PIN 12 // Strobe pin 
#define RESET_PIN 13 // Reset Pin

CRGB leds[NUM_LEDS];
int currentValue = 0;
int values[] = {0,0};

void setup() 
{
  /* Initialize FastLED */
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600); // print to serial monitor
  FastLED.show(); // Initialize all pixels to 'off'

  /* Set pinmodes */
  pinMode(ANALOG_PIN_L, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STOMP_PIN, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH); 

}

void loop() 
{  
  for(int i=0; i<2; i++){
   if(Serial.available()){
    int incomingValue = Serial.read();
    
    values[currentValue] = incomingValue;
    //values[1]=incomingValue;

    currentValue++;
    if(currentValue > 1){
      currentValue = 0;
    }
    
    // after this point values[]
    // has the most recent set of
    // all values sent in from Processing
  } 
  }
    
    for (int k = 0; k < ROWS; k++) { //columns
       byte odd=k%2;
       for(int j=0; j<COLUMNS; j++) { 
         byte jrev;
         if(odd==1){jrev=(COLUMNS-1)-j;}
         else{jrev=j;}
         rows(j,k,jrev,odd);
       }//rows
       
     }//for

     FastLED.show();

}

void rows(int j, int k, byte jrev, byte odd) {
  int color=100;
  int bright=0;
  int x=10-values[0];
  int y=25-values[1];
  int r_x=x-k;  //radiusx
  int r_y=y-jrev;  //radiusy
  if((abs(r_x)==1 && r_y==0) || (r_x==0 && abs(r_y)==1)){
  //if((r_x<2 && r_y==0) || (r_x==0 && r_y<2)) {
    color=50+jrev*8-k*25;
    bright=55;
  }
  if(k==x && jrev==y){
    color=200;
    bright=255;
  }
  leds[k*COLUMNS+j+1]=CHSV(color-k*8,187,bright );
}

