#include <FastLED.h>
#include <Dot.h>


#define NUM_LEDS 251 // Number of LED's in the strip
#define ROWS 10 //# of leds in each row (in vertical pos)
#define COLUMNS 25 //# of leds in each column
#define TIMER 300
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
int oldvalx=0;
int brights[ROWS];

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

  for(int i=0; i<ROWS; i++) {
    brights[i]=0;
  }
}

int z=0;//colorshift
int counter=0; //counter to turn off LEDs when no motion
int x=0;
int y=0;
int oldx=0;
int oldy=0;

void rows(int j, int k, byte jrev, byte odd) { //this function runs for EVERY SINGLE LED
  int color=(50+jrev*8-k*25+z)%255;
  
  /*oldx=x;
  x=10-values[0];
  oldy=y;
  y=25-values[1];
  
  int r_x=x-k;  //radiusx
  int r_y=y-jrev;  //radiusy
  if(r_x==0){
    brights[k]=255;
  }*/
  int bright = brights[k];
  //if(counter>TIMER){bright=0;}
  leds[k*COLUMNS+j+1]=CHSV(color-k*8,187,bright);
}

void loop() {  
  for(int i=0; i<2; i++){
   if(Serial.available()){
    int incomingValue = Serial.read();

    if(currentValue==0){
      oldvalx=values[0];
    }
   
    values[currentValue] = incomingValue;
    //values[1]=incomingValue;

    if(oldvalx!=values[0]){
      counter=0;
    }

    currentValue++;
    if(currentValue > 1){
      currentValue = 0;
    }
    
    // after this point values[]
    // has the most recent set of
    // all values sent in from Processing
   } 
  }

  x=10-values[0];
  
    
    for (int k = 0; k < ROWS; k++) { // 0-9 columns
      if(x==k){
        brights[k]=255;
      }
      if(brights[k]>0){
        brights[k] = brights[k] -5;
      }
       byte odd=k%2;
       for(int j=0; j<COLUMNS; j++) { 
         byte jrev;
         if(odd==1){jrev=(COLUMNS-1)-j;}
         else{jrev=j;}
         rows(j,k,jrev,odd);
       }//rows
       
     }//for

     if(z<255){z++;}
     else{z=0;} //color shifter    

     counter++;

     FastLED.show();
}




