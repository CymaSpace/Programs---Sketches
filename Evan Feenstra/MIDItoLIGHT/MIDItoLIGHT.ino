#include "FastLED.h"

#define NUM_LEDS 251 // Number of LED's in the strip
#define DATA_PIN 13
#define ROWS 25
#define COLUMNS 10

CRGB leds[NUM_LEDS];
int bright=0;
int voices[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int v[12]; // organized note array
int lights[NUM_LEDS];
int oldlights[NUM_LEDS];

int noteToNum (int note) {
  int noteLetter = note % 12;
  int ring;
  switch (noteLetter) {
    case 0: // C
      Serial.println("C");
      ring=0;
      break;
    case 7: // G
      Serial.println("G");
      ring=11;
      break;
    case 2: // D
      Serial.println("D");
      ring=10;
      break;
    case 9: // A
      Serial.println("A");
      ring=9;
      break;
    case 4: // E
      Serial.println("E");
      ring=8;
      break;
    case 11: // B
      Serial.println("B");
      ring=7;
      break;
    case 6:  // F#
      Serial.println("F#");
      ring=6;
      break;
    case 1: // Db
      Serial.println("C#");
      ring=5;
      break;
    case 8: // Ab
      Serial.println("G#");
      ring=4;
      break;
    case 3: // Eb
      Serial.println("D#");
      ring=3;
      break;
    case 10: //Bb
      Serial.println("A#");
      ring=2;
      break;
    case 5: // F
      Serial.println("F");
      ring=1;
      break;
  } //end switch statement
  return ring;
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  bright=99;
  int ring = noteToNum(note);
  int color=ring*21.25;
  voices[ring]=color;
  /*Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();*/
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  int ring = noteToNum(note);
  voices[ring]=-1;
  /*Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.print(velocity, DEC);
  Serial.println();*/
}

void setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
}

int checkVoices() {
  int num=0;
  for(int i=0; i<12; i++) {
    if(voices[i]>-1){
      v[num]=voices[i];
      num++;
    }
  }
  if(num==0){bright=0;}
  return num;
}

void propogate() {
  
}

void loop() {
  int numVoices = checkVoices();
  int binSize = ROWS/numVoices;

  for(int i=0; i<ROWS; i++) { //one column
        int x;
        //if(odd==1){x=(ROWS-2-i)/binSize;}
        x=i/binSize;
        int color=v[x];
        lights[i]=color;
        oldlights[i]=lights[i];
        leds[i]=CHSV(lights[i],255,bright);
      }
   // END FIRST COLUMN
      FastLED.show();
      
  for(int j=1; j<COLUMNS; j++) {
    int odd=j%2;
    for(int i=0; i<ROWS; i++) {
      if(odd==0){
        lights[i+ROWS*j]=oldlights[i+ROWS*(j-1)];
        oldlights[i+ROWS*j]=lights[i+ROWS*j];
      }
      else{
        lights[i+ROWS*j]=oldlights[ROWS-i-1+ROWS*(j-1)];
        oldlights[i+ROWS*j]=lights[ROWS-i-1+ROWS*j];
      }
      leds[i+ROWS*j]=CHSV(lights[i+ROWS*j],255,bright);
    }
    FastLED.show();
    
  } // end j loop
  
  usbMIDI.read();
  /*if(bright>0){
    bright--;
  }*/
}




