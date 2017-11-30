/***
 *                                                                                                                                        
 *    MMMMMMMM               MMMMMMMM   SSSSSSSSSSSSSSS         GGGGGGGGGGGGGEEEEEEEEEEEEEEEEEEEEEE     QQQQQQQQQ     77777777777777777777
 *    M:::::::M             M:::::::M SS:::::::::::::::S     GGG::::::::::::GE::::::::::::::::::::E   QQ:::::::::QQ   7::::::::::::::::::7
 *    M::::::::M           M::::::::MS:::::SSSSSS::::::S   GG:::::::::::::::GE::::::::::::::::::::E QQ:::::::::::::QQ 7::::::::::::::::::7
 *    M:::::::::M         M:::::::::MS:::::S     SSSSSSS  G:::::GGGGGGGG::::GEE::::::EEEEEEEEE::::EQ:::::::QQQ:::::::Q777777777777:::::::7
 *    M::::::::::M       M::::::::::MS:::::S             G:::::G       GGGGGG  E:::::E       EEEEEEQ::::::O   Q::::::Q           7::::::7 
 *    M:::::::::::M     M:::::::::::MS:::::S            G:::::G                E:::::E             Q:::::O     Q:::::Q          7::::::7  
 *    M:::::::M::::M   M::::M:::::::M S::::SSSS         G:::::G                E::::::EEEEEEEEEE   Q:::::O     Q:::::Q         7::::::7   
 *    M::::::M M::::M M::::M M::::::M  SS::::::SSSSS    G:::::G    GGGGGGGGGG  E:::::::::::::::E   Q:::::O     Q:::::Q        7::::::7    
 *    M::::::M  M::::M::::M  M::::::M    SSS::::::::SS  G:::::G    G::::::::G  E:::::::::::::::E   Q:::::O     Q:::::Q       7::::::7     
 *    M::::::M   M:::::::M   M::::::M       SSSSSS::::S G:::::G    GGGGG::::G  E::::::EEEEEEEEEE   Q:::::O     Q:::::Q      7::::::7      
 *    M::::::M    M:::::M    M::::::M            S:::::SG:::::G        G::::G  E:::::E             Q:::::O  QQQQ:::::Q     7::::::7       
 *    M::::::M     MMMMM     M::::::M            S:::::S G:::::G       G::::G  E:::::E       EEEEEEQ::::::O Q::::::::Q    7::::::7        
 *    M::::::M               M::::::MSSSSSSS     S:::::S  G:::::GGGGGGGG::::GEE::::::EEEEEEEE:::::EQ:::::::QQ::::::::Q   7::::::7         
 *    M::::::M               M::::::MS::::::SSSSSS:::::S   GG:::::::::::::::GE::::::::::::::::::::E QQ::::::::::::::Q   7::::::7          
 *    M::::::M               M::::::MS:::::::::::::::SS      GGG::::::GGG:::GE::::::::::::::::::::E   QQ:::::::::::Q   7::::::7           
 *    MMMMMMMM               MMMMMMMM SSSSSSSSSSSSSSS           GGGGGG   GGGGEEEEEEEEEEEEEEEEEEEEEE     QQQQQQQQ::::QQ77777777            
 *                                                                                                              Q:::::Q                   
 *                                                                                                               QQQQQQ                   
 *                                                                                                                                        
 *                                                                                                                                        
 */
 
#include<FastLED.h>
#define NUM_LEDS 112

// The leds
CRGB leds[NUM_LEDS];

// MSGEQ7
#include "MSGEQ7.h"
#define pinAnalogLeft 1
#define pinAnalogRight 0
#define pinReset 13
#define pinStrobe 12
#define MSGEQ7_INTERVAL ReadsPerSecond(60)
#define MSGEQ7_SMOOTH true
#define FLOOR 65 // increase this if you have noise when no signal input
#define MAX_IN 255 // change this depending on the signal level input.

// LED MAPPING
#define COLUMN_SIZE 15 // numLEDs per column.

// LED COLORS


CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;

void setup() {
  delay(3000);
  LEDS.addLeds<WS2812,6,RGB>(leds,NUM_LEDS);
  LEDS.clear();
  LEDS.setBrightness(46);
  BlackOut(); // set all leds to CRGB::Black
  LEDS.show();
  MSGEQ7.begin();
}


void loop() {
        // analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

  // Led strip output
  if (newReading) {
    int led6 = map(MSGEQ7.get(MSGEQ7_6), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[led6] = CRGB::Fuchsia;
    for (int i = 0; i < led6; i++) {leds[i] = CRGB::Purple;}
    
    int led5 = map(MSGEQ7.get(MSGEQ7_5), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[31-led5] = CRGB::DarkBlue;
    for (int i = 0; i < led5; i++) {leds[32-i] = CRGB::Blue;}
    
    int led4 = map(MSGEQ7.get(MSGEQ7_4), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[32+led4] = CRGB::PowderBlue;
    for (int i = 0; i < led4; i++) { leds[32+i] = CRGB::Turquoise;}
        
    int led3 = map(MSGEQ7.get(MSGEQ7_3), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[63-led3] = CRGB::ForestGreen;
    for (int i = 0; i < led3; i++) {leds[63-i] = CRGB::Green;}
    
    int led2 = map(MSGEQ7.get(MSGEQ7_2), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[64+led2] = CRGB::Yellow;
    for (int i = 0; i < led2; i++) {leds[64+i] = CRGB::Gold;}

    int led1 = map(MSGEQ7.get(MSGEQ7_1), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[95-led1] = CRGB::DarkOrange;
    for (int i = 0; i < led1; i++) {leds[95-i] = CRGB::OrangeRed;}
    
    int led0 = map(MSGEQ7.get(MSGEQ7_0), FLOOR, MAX_IN, 1, COLUMN_SIZE);
    leds[96+led0] = CRGB::Red;
    for (int i = 0; i < led0; i++) {leds[96+i] = CRGB::Red; }
    
  LEDS.show();
  BlackOut();     
 }
}

void BlackOut(void) {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}
