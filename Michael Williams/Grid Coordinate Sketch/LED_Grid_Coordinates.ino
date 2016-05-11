// In this sketch, a user can enter LED grid dimensions. The program
// will find the center LED, and propigate outwards.
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19

#include <FastLED.h>

#define DATA_PIN  6

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 64

// Grid Parameters
#define grid_x 10
#define grid_y 10

int x_input = 1;
int y_input = 1;

// Set color value to full saturation and value. Set the hue to 0
CHSV color(0, 255, 255);
CRGB leds[grid_x * grid_y + 1]; // Represents LED strip

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, grid_x * grid_y + 1);
  FastLED.show();

}

  int a;
  int b;
  int pixel;


int light_pixel (int x_input, int y_input){
  if ((y_input % 2) == 0){
    a = y_input * grid_y;
    b = a - x_input;
  }else{
    b = ((y_input - 1) * grid_y) + x_input - 1;
  }

  return b;
}


  
void loop() {
  
  for (y_input = 1 ; y_input < 11 ; y_input++,x_input++){
        leds[light_pixel(x_input, y_input)] = CRGB::Red;
        FastLED.show();
        delay(1000);      

  }
 
  
  while (true == true){}
}



