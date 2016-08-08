#include <FastLED.h>
#include "EEPROM.h"

/* Output pin definitions */
#define NUM_LEDS 100 // Number of LED's in the strip
#define DATA_PIN 6 // Data out
#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define STOMP_PIN 5 // The pin connected to the stomp button
#define STROBE_PIN 12 // Strobe pin
#define RESET_PIN 13 // Reset Pin

/* Sensitivity variables, refresh variables, and start/end points */
#define LED_STACK_SIZE 10 // How many LED's in each stack
#define SOUND_SMOOTHING_LENGTH 2 // How many sound samples to use for smoothing
#define MAX_AMPLITUDE 2000 // Maximum possible amplitude value
#define MIN_AMPLITUDE 500 // Lowest possible amplitude value (Higher number causes there to be more blank LED's)

#define TICKS 100
#define MILLIS_PER_TICK (1000 / TICKS)

int monomode; // Used to duplicate the left single for manual input
int refresh_rate; // Refresh rate of the animation
int refresh_counter = 0; // Looping variable for refresh loop
int sensitivity; // Sensitivity value
int start_hue = 192;

int next_tick = 0; // Refresh rate of the animation

/* Represents the left and right LED color values.
 * In the case of an odd number of LED's, you need to adjust these
 * values and the values of RIGHT_START_POINT, LEFT_START_POINT, etc.
 */

int LED_stack[LED_STACK_SIZE] = {0};

// Set color value to full saturation and value. Set the hue to 0
CHSV color(0, 255, 255);
CRGB leds[NUM_LEDS]; // Represents LED strip

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
  }

  return spectrum_total;
}

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

  float max = (MAX_AMPLITUDE - MIN_AMPLITUDE);
  if(value > max) value = max;
  float ratio = ((float)(value / max));
  if(ratio == 0) {
    color.val = 0;
  } else {
    color.val = 255;
  }
  color.saturation = 255;
  color.hue = start_hue + (-ratio * start_hue);
  leds[position] = color;
}

/*  Push a new LED color value onto the beginning of the stack.
 *  The last LED color value is discarded. This is the primary
 *  function relating to the propagation behavior.
 */
void push_stack(int stack[], int value) {
  int sum = 0;
  for (int i = 0; i < SOUND_SMOOTHING_LENGTH; i++) {
    sum += stack[i];
  }
  sum += value;
  value = sum / (SOUND_SMOOTHING_LENGTH + 1);

  for(int i = (LED_STACK_SIZE - 1); i >= 0; --i) {
    stack[i] = stack[i - 1];
  }
  stack[0] = value;
}


void setup() {

  // Instantiate Neopixels with FastLED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, 251);
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
  int current_time = millis();

  if (current_time > next_tick) {
    next_tick = current_time + MILLIS_PER_TICK;
  } else {
    return;
  }

  // Reset EQ7 chip
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  // Get the sum of the amplitudes of all 7 frequency bands
  int amp_sum_L = get_freq_sum(ANALOG_PIN_L);
  int amp_sum_R = get_freq_sum(ANALOG_PIN_R);

  push_stack(LED_stack, (amp_sum_L + amp_sum_R) / 2);

  for(int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      set_LED_color((i * 10) + j, LED_stack[i]);
    }
  }

  // Show the new LED values
  FastLED.show();
}
