#include <FastLED.h>
#include "EEPROM.h"

/* Output pin definitions */
<<<<<<< HEAD
#define NUM_LEDS 269
=======
<<<<<<< HEAD
#define NUM_LEDS 71 // Number of LED's in the strip
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
#define RIGHT_END_POINT (NUM_LEDS -1) // Generally the end of the right side is the last LED
#define LED_STACK_SIZE (NUM_LEDS / 2) // How many LED's in each stack
#define MAX_AMPLITUDE 4700 // Maximum possible amplitude value
#define MAX_AMPLITUDE_MULTIPLIER 380
#define MIN_AMPLITUDE 420 // Lowest possible amplitude value (Higher number causes there to be more blank LED's)
#define MIN_AMPLITUDE_MULTIPLIER 200
#define SENSITIVITY_MULTIPLIER 200 // Higher = range of sensitivity values on pot is lower

int monomode; // Used to duplicate the left single for manual input
int refresh_rate; // Refresh rate of the animation
int refresh_counter = 0; // Looping variable for refresh loop
int sensitivity; // Sensitivity value
int max_amplitude;
int start_hue = 0;

/* Represents the left and right LED color values.
 * In the case of an odd number of LED's, you need to adjust these
 * values and the values of RIGHT_START_POINT, LEFT_START_POINT, etc.
 */
=======
#define NUM_LEDS 300
>>>>>>> origin/master
#define DATA_PIN 6
#define ANALOG_PIN_L 1
#define ANALOG_PIN_R 0
#define REFRESH_POT_PIN 2
#define SENSITIVITY_POT_PIN 3
#define STOMP_PIN 5
#define STROBE_PIN 12
#define RESET_PIN 13
#define REFRESH_DIVISOR 80.
#define SENSITIVITY_DIVISOR 100.
#define LEFT_START_POINT ((NUM_LEDS / 2) - 1)
#define LEFT_END_POINT 0
#define RIGHT_START_POINT ((NUM_LEDS / 2))
#define RIGHT_END_POINT (NUM_LEDS -1)
#define LED_STACK_SIZE (NUM_LEDS / 2)
#define MAX_AMPLITUDE 5000
#define MIN_AMPLITUDE 500
#define MAX_SENSITIVITY 3000
#define MIN_SENSITIVITY 0
int monomode;
int refresh_rate;
int refresh_counter = 0;
int sensitivity;
>>>>>>> 5753a9a2de08e7b53d299f473ff308a9279a06a3

int left_LED_stack[NUM_LEDS / 2] = {0};
int right_LED_stack[NUM_LEDS / 2] = {0};

CHSV color(0, 255, 255);
CRGB leds[NUM_LEDS];

void setup() {
<<<<<<< HEAD

  // Instantiate Neopixels with FastLED
=======
  Serial.begin(9600); // print to serial monitor
  
>>>>>>> 5753a9a2de08e7b53d299f473ff308a9279a06a3
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();

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

  // Set local loop variables
  int amp_sum_L = 0;
  int amp_sum_R = 0;
  int i;
  int stack_loop = 0;

  // Reset EQ7 chip
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);
  
  // Change color mode if stomp button is pressed
  if(stomp_pressed()) {
    change_color_mode();
<<<<<<< HEAD
    delay(100); // Keeps the pedal from switching colors too quickly
=======
>>>>>>> 5753a9a2de08e7b53d299f473ff308a9279a06a3
  }

  amp_sum_L = get_freq_sum(ANALOG_PIN_L);
  amp_sum_R = get_freq_sum(ANALOG_PIN_R);

  /* If monomode is active, make both L and R equal to the
     value of L */
  if(monomode) amp_sum_L = amp_sum_R;

<<<<<<< HEAD
  /*  SENSITIVITY_DIVISOR lowers the range of the possible pot values
   *  If the value is higher than the max or lower than the min,
   *  set it to the max or min respectively.
   */
  sensitivity = (analogRead(SENSITIVITY_POT_PIN) / SENSITIVITY_DIVISOR) * SENSITIVITY_MULTIPLIER;
  max_amplitude = MAX_AMPLITUDE - (((1023 - analogRead(SENSITIVITY_POT_PIN)) / SENSITIVITY_DIVISOR) * MAX_AMPLITUDE_MULTIPLIER);

  // REFRESH_DIVISOR lowers the range of possible pot values
=======
  sensitivity = (analogRead(SENSITIVITY_POT_PIN) / SENSITIVITY_DIVISOR) * 800;
  if(sensitivity > MAX_SENSITIVITY) sensitivity = MAX_SENSITIVITY;
  if(sensitivity < MIN_SENSITIVITY) sensitivity = MIN_SENSITIVITY;
>>>>>>> 5753a9a2de08e7b53d299f473ff308a9279a06a3
  refresh_rate = analogRead(REFRESH_POT_PIN) / REFRESH_DIVISOR;

  if(refresh_counter >= refresh_rate) {
    refresh_counter = 0;
        
    push_stack(left_LED_stack, amp_sum_L);
    push_stack(right_LED_stack, amp_sum_R);
    
    for(i = LEFT_START_POINT; i >= LEFT_END_POINT; --i) {
      set_LED_color(i, left_LED_stack[stack_loop]);
      ++stack_loop;
    }
    
    stack_loop = 0;
    for(i = RIGHT_START_POINT; i <= RIGHT_END_POINT; ++i) {
      set_LED_color(i, right_LED_stack[stack_loop]);
      ++stack_loop;
    }
        
    FastLED.show();
  }

  ++refresh_counter;
  
}

// Check if stomp button is being pressed
int stomp_pressed() {
  if (digitalRead(STOMP_PIN) == HIGH){
    return 1;
  } else {
    return 0;
  }
}

void change_color_mode() {
  start_hue += 85;
  if(start_hue >= 255) {
    start_hue = start_hue - 255;
  }
}

// Read in and sum amplitudes for the 7 frequency bands
int get_freq_sum(int pin) {

  int i;
  int spectrum_values[7];
  int spectrum_total = 0;

  //get readings from chip, sum freq values
  for (i = 0; i < 7; i++) {
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(30); // to allow the output to settle

    spectrum_values[i] = analogRead(pin);
    spectrum_total += spectrum_values[i];
    
    // strobe to the next frequency
    digitalWrite(STROBE_PIN, HIGH); 
   
  }//for i

  return spectrum_total;
}

<<<<<<< HEAD
/* Sets led 'position' to 'value' and converts the value to an HSV value.
 * Compared to the A3 code, this code produces color values closer to white.
 * The A3 code always has only two colors lit at a time. For example red and 
 * green but not blue or blue and red but not green.
 * As a result, the colors are more like pastel hues than bold hues.
 * Another option would be to do something similar to the limited RGB values from A3.
 */
void set_LED_color(int position, int value) {
  // If lower than min amplitude, set to min amplitude
  if(value <= MIN_AMPLITUDE) {
    value = MIN_AMPLITUDE;
  }
  
  // Subtract min amplitude so lowest value is 0
  value -= MIN_AMPLITUDE;
  
  float max = (max_amplitude - MIN_AMPLITUDE);
  if(value > max) value = max;
  float ratio = ((float)(value / max));
=======
// Update color values for LED strip
void update_LED_positions() {
  // Update LED propagation here
}

void set_LED_color(int position, int value) {
  if(value < MIN_AMPLITUDE) value = 0;
  float max = MAX_AMPLITUDE - sensitivity;
  float ratio = ((float)value / max);
>>>>>>> 5753a9a2de08e7b53d299f473ff308a9279a06a3
  if(ratio == 0) {
    color.val = 0;
  } else {
    color.val = 255;
  }
  color.saturation = 255;
  color.hue = start_hue + (ratio * 255);
  leds[position] = color;
}

void push_stack(int stack[], int value) {
  int i;
  for(i = (LED_STACK_SIZE - 1); i >= 1; --i) {
    stack[i] = stack[i - 1];
  }
  stack[0] = value;
}
