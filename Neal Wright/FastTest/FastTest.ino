#include <FastLED.h>
#include "EEPROM.h"

/* Output pin definitions */
#define NUM_LEDS 300
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

int left_LED_stack[NUM_LEDS / 2] = {0};
int right_LED_stack[NUM_LEDS / 2] = {0};

CHSV color(0, 255, 255);
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600); // print to serial monitor
  
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
  }

  amp_sum_L = get_freq_sum(ANALOG_PIN_L);
  amp_sum_R = get_freq_sum(ANALOG_PIN_R);

  /* If monomode is active, make both L and R equal to the
     value of L */
  if(monomode) amp_sum_L = amp_sum_R;

  sensitivity = (analogRead(SENSITIVITY_POT_PIN) / SENSITIVITY_DIVISOR) * 800;
  if(sensitivity > MAX_SENSITIVITY) sensitivity = MAX_SENSITIVITY;
  if(sensitivity < MIN_SENSITIVITY) sensitivity = MIN_SENSITIVITY;
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

// Change which color values correspond to amplitude values
void change_color_mode() {
  // @TO-DO: Finish this function
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

// Update color values for LED strip
void update_LED_positions() {
  // Update LED propagation here
}

void set_LED_color(int position, int value) {
  if(value < MIN_AMPLITUDE) value = 0;
  float max = MAX_AMPLITUDE - sensitivity;
  float ratio = ((float)value / max);
  if(ratio == 0) {
    color.val = 0;
  } else {
    color.val = 255;
  }
  color.hue = 255 - (ratio * 255);
  leds[position] = color;
}

void push_stack(int stack[], int value) {
  int i;
  for(i = (LED_STACK_SIZE - 1); i >= 1; --i) {
    stack[i] = stack[i - 1];
  }
  stack[0] = value;
}

