#include "Arduino.h"

void runleds(){
  
   // Set local loop variables
  int i; // Loop var
  int stack_loop = 0;
  // REFRESH_DIVISOR lowers the range of possible pot values
  refresh_rate = analogRead(REFRESH_POT_PIN) / REFRESH_DIVISOR;

  // Run this code based on the refresh rate
  if(refresh_counter >= refresh_rate) {

    // Start by resetting the refresh counter
    refresh_counter = 0;

    /*  Push the new values onto the stack of LED's for each side
     *  This moves each LED value up the strip by 1 LED and drops the
     *  last value in the stack. This is the code that effects the propagation
     */
    push_stack(left_LED_stack, amp_sum_L);
    push_stack(right_LED_stack, amp_sum_R);

    /*  Set the LED values based on the left and right stacks
     *  This is a reverse loop because the left side LED's travel toward
     *  LED 0.
     */
    for(i = LEFT_START_POINT; i >= LEFT_END_POINT; --i) {
      set_LED_color(i, left_LED_stack[stack_loop]);
      ++stack_loop;
    }

    // Reset and reuse stack_loop var
    stack_loop = 0;

    /*  LED's on the right travel towards the last LED in the strand 
     *  so the loop increments positively
     */ 
    for(i = RIGHT_START_POINT; i <= RIGHT_END_POINT; ++i) {
      set_LED_color(i, right_LED_stack[stack_loop]);
      ++stack_loop;
    }

    leds[0] = left_LED_stack[0];

    // Show the new LED values
    FastLED.show();
  }

  // Increase the refresh counter
  ++refresh_counter;
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
  
  float max = (max_amplitude - MIN_AMPLITUDE);
  if(value > max) value = max;
  float ratio = ((float)(value / max));
  if(ratio == 0) {
    color.val = 0;
  } else {
    color.val = 255;
  }
  color.saturation = 255;
  color.hue = start_hue + ((ratio * 255) * 2);
  leds[position] = color;
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


/*  Push a new LED color value onto the beginning of the stack.
 *  The last LED color value is discarded. This is the primary 
 *  function relating to the propagation behavior.
 */
void push_stack(int stack[], int value) {
  int i;
  for(i = (LED_STACK_SIZE - 1); i >= 0; --i) {
    stack[i] = stack[i - 1];
  }
  stack[0] = value;
}
