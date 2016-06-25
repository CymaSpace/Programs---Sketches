#include <FastLED.h>
#include "EEPROM.h"

/* Output pin definitions */
#define NUM_LEDS 55 // Number of LED's in the strip
#define DATA_PIN 6 // Data out
#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define SENSITIVITY_POT_PIN 3 // Right pot
#define STOMP_PIN 5 // The pin connected to the stomp button
#define STROBE_PIN 12 // Strobe pin 
#define RESET_PIN 13 // Reset Pin

/* Sensitivity variables, refresh variables, and start/end points */
#define REFRESH_DIVISOR 80. // Higher = range of refresh values is lower
#define SENSITIVITY_DIVISOR 100. // Higher = range of sensitivity values on pot is lower
#define LEFT_START_POINT ((NUM_LEDS / 2)) // Starting LED for left side
#define LEFT_END_POINT 1 // Generally the end of the left side is the first LED
#define RIGHT_START_POINT ((NUM_LEDS / 2) + 1) // Starting LED for the right side
#define RIGHT_END_POINT (NUM_LEDS - 1) // Generally the end of the right side is the last LED
#define LED_STACK_SIZE (NUM_LEDS / 2) // How many LED's in each stack
#define MAX_AMPLITUDE 4700 // Maximum possible amplitude value
#define MAX_AMPLITUDE_MULTIPLIER 380
#define MIN_AMPLITUDE 545 // Lowest possible amplitude value (Higher number causes there to be more blank LED's)
#define MIN_AMPLITUDE_MULTIPLIER 200
#define SENSITIVITY_MULTIPLIER 200 // Higher = range of sensitivity values on pot is lower

int monomode; // Used to duplicate the left single for manual input
int refresh_rate; // Refresh rate of the animation
int refresh_counter = 0; // Looping variable for refresh loop
int sensitivity; // Sensitivity value
int max_amplitude;
int start_hue = 0;
int amp_sum_L = 0;
int amp_sum_R = 0;
  
/* Represents the left and right LED color values.
 * In the case of an odd number of LED's, you need to adjust these
 * values and the values of RIGHT_START_POINT, LEFT_START_POINT, etc.
 */

int left_LED_stack[NUM_LEDS / 2] = {0};
int right_LED_stack[NUM_LEDS / 2] = {0};
int left_LED_stack_A4BW[NUM_LEDS / 2] = {0};
int right_LED_stack_A4BW[NUM_LEDS / 2] = {0};

// Set color value to full saturation and value. Set the hue to 0
CHSV color(0, 255, 255);
CRGB leds[NUM_LEDS]; // Represents LED strip
CRGB leds_A4BW[NUM_LEDS]; // Represents LED strip

//____________Function declarations______
int get_freq_sum(int pin);
void set_LED_color(int position, int value);
void change_color_mode();
int stomp_pressed();
void push_stack(int stack[], int value);
void getAudiomsg();
void setsensitivity();
void runleds();
void runleds_A4BW();
void set_LED_color_A4BW(int position, int value);
void change_color_mode_A4BW();
void push_stack_A4BW(int stack[], int value);
//_______________________________________



void setup() {
  Serial.begin(9600);
  // Instantiate Neopixels with FastLED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, DATA_PIN+1>(leds_A4BW, NUM_LEDS);
  FastLED.show();

  // Clear any old values on EEPROM
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM

  // Set pin modes
  pinMode(ANALOG_PIN_L, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STOMP_PIN, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH); 

  // If stomp is being pressed during setup, set monomode to True
  if (digitalRead(STOMP_PIN) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }

  // Set monomode based on the EEPROM state
  monomode = EEPROM.read(1);
}

void loop() {
  getAudiomsg();  //sets ampsum left and right value
  runleds();   //uses ampsum left and right and sets leds
  runleds_A4BW();   //uses ampsum left and right and sets leds();
  runleds();   
}
