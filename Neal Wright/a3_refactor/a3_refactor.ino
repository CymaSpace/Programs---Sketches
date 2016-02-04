#include <Adafruit_NeoPixel.h>
#include "EEPROM.h"
#define PIN 6
#define CNT_LIGHTS 56

// Instantiate Neopixel Strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_GRB + NEO_KHZ800); 

/* === Fixed settings === */

// Hardware Variables
int analog_pin_L = 1; // read from multiplexer using analog input 0
int analog_pin_R = 0; // read from multiplexer using analog input 0
int analog_pinpot = 2;// dial for speed
int analog_pot_range = 3;// dial for range
int stomp = 5;
int strobe_pin = 12; // strobe is attached to digital pin 2
int reset_pin = 13; // reset is attached to digital pin 3

// LED Variables
int color_state = 0;
int refresh = 10;
int refresh_counter = 0;
int tmp_refresh_adj = 0;
int global_brightness = 255; // Sets global brightness, i.e. 64 is 1/4 brightness.
int left_end_point = 0;
int left_start_point = ((CNT_LIGHTS/2)-1);
int right_start_point = ((CNT_LIGHTS/2));
int right_end_point = (CNT_LIGHTS-1);

// Variable to test whether monomode is active
boolean monomode = 0;

void setup() 
{  
  Serial.begin(9600); // print to serial monitor
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM

  // Set pin modes
  pinMode(analog_pin_L, INPUT);
  pinMode(analog_pin_R, INPUT);
  pinMode(stomp, INPUT);
  pinMode(analog_pin_R, INPUT);
  pinMode(strobe_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);

  digitalWrite(reset_pin, LOW);
  digitalWrite(strobe_pin, HIGH); 

  // If stomp is being pressed during setup, set monomode to True
  if (digitalRead(stomp) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }

  // Set monomode based on the EEPROM state
  monomode = EEPROM.read(1);
}
 
void loop() 
{  
  // If stomp button is pressed, change color_state
  check_color_state();

  digitalWrite(reset_pin, HIGH);
  digitalWrite(reset_pin, LOW);

  int cur_sum_L = 0;
  int cur_sum_R = 0;

  // Represents all of the LED freq sum values in the LED strip
  int cur_LED_vals[CNT_LIGHTS];
  int prev_LED_vals[CNT_LIGHTS];

  // Get spectrum totals for L and R
  cur_sum_L = read_freq_sum(analog_pin_L);
  cur_sum_R = read_freq_sum(analog_pin_R);

  // If monomode is active, make L equal to R
  if (monomode == 1){
    cur_sum_L = cur_sum_R;
  }

  // Update LED values, moving them down the line based on amplitude of all frequencies
  update_led_positions(cur_LED_vals, prev_LED_vals, cur_sum_L, cur_sum_R);
  set_pixel_colors(cur_LED_vals);       

  strip.show();
}

int check_color_state() {
  if (digitalRead(stomp) == HIGH){
    delay(100);
    if(color_state < 2){color_state++;}
    else if(color_state == 2){color_state=0;}
  }
}

int read_freq_sum(int pin) {

  int i;
  int spectrum_values[7];
  int spectrum_total = 0;

  //get readings from chip, sum freq values
  for (i = 0; i < 7; i++)
  {
    digitalWrite(strobe_pin, LOW);
    delayMicroseconds(30); // to allow the output to settle

    spectrum_values[i] = analogRead(pin);
    adjust_refresh_rate(spectrum_values[i], i);
    
    spectrum_total += spectrum_values[i];
    
    // strobe to the next frequency
    digitalWrite(strobe_pin, HIGH); 
   
  }//for i

  return spectrum_total;
}

void adjust_refresh_rate(int amplitude, int freq_band) {
  if( amplitude > 400 && freq_band <= 1 )
    tmp_refresh_adj += 23;
}

void update_led_positions(int cur_LED_vals[], int prev_LED_vals[], int cur_sum_L, int cur_sum_R) {

  int i;
  int use_refresh = 10;
  float pot_value = analogRead(analog_pinpot); //for use of dial

  if(pot_value < 1000) {
    if (pot_value > 20){
      use_refresh = pot_value / 50;
    } else {
      use_refresh = 0;
    }
  }

  // Constrain temporary refresh rate adjustment to between 0 and 200
  if(tmp_refresh_adj < 0)
    tmp_refresh_adj = 0;
  if(tmp_refresh_adj > 200)
    tmp_refresh_adj = 200;

  refresh_counter++;

  if(refresh_counter >= (use_refresh - round(tmp_refresh_adj * .1))) {
    
    //reset the counter
    refresh_counter = 0;

    // Decay the temporary refresh rate adjustment
    tmp_refresh_adj -= 1;

    //save the history - RIGHT SIDE
    for (i = right_start_point; i <= right_end_point; i++) {
      prev_LED_vals[i] = cur_LED_vals[i-1];
    }//for
    for (i = right_start_point; i <= right_end_point; i++) {
      cur_LED_vals[i] = prev_LED_vals[i];
    }//for
    
    //save the history - LEFT SIDE
    for (i = left_start_point; i >= left_end_point; i--) {
      prev_LED_vals[i] = cur_LED_vals[i+1];
    }//for
    for (i = left_start_point; i >= left_end_point; i--) {
      cur_LED_vals[i] = prev_LED_vals[i];
    }//for

    cur_LED_vals[left_start_point] = cur_sum_L;
    cur_LED_vals[right_start_point] = cur_sum_R;
    
  }//if refresh
}

float get_wave_length(int num)
{
  float min_val = 500;
  float max_val = 4700;
  float min_wave = 350;
  float max_wave = 650;
  float pot_range = analogRead(analog_pot_range);
  max_val = pot_range * 5;
  min_val = pot_range / 2;
  if(num > max_val)
    max_val = num;

  return ((num - min_val) / (max_val-min_val) * (max_wave - min_wave)) + min_wave;

}

void get_RGB(float wave_value, int color_array[])
{

  float rz = 0, gz = 0, bz = 0;
  int r,g,b;
  
  // If wave value is within a certain range, set color value (0.0 - 1.0)
  if(wave_value >380 && wave_value <=439) {
    rz = (wave_value-440)/(440-380);
    gz = 0;
    bz = 1;
  }
  
  if(wave_value >=440 && wave_value <=489) {
    rz = 0;
    gz = (wave_value-440)/(490-440);
    bz = 1;
  }
  
  if(wave_value >=490 && wave_value <=509) {
    rz = 0;
    gz = 1;
    bz = (wave_value-510)/(510-490);
  }
  
  if(wave_value >=510 && wave_value <=579) {
    rz = (wave_value-510)/(580-510);
    gz = 1;
    bz = 0;
  }
  
  if(wave_value >=580 && wave_value <=644) {
    rz = 1;
    gz = (wave_value-645)/(645-580);
    bz = 0;
  }
  
  if(wave_value >=645 && wave_value <=780) {
    rz = 1;
    gz = 0;
    bz = 0;
  }
  
  // Convert RGB decimal value to 0-254 value
  r = rz * 255;
  b = bz * 255;
  g = gz * 255;

  // Shift RGB values based on color_state  
  if(color_state == 0){
    color_array[0] = b;
    color_array[1] = g;
    color_array[2] = r;
  }
  if(color_state == 1){
    color_array[0] = r;
    color_array[1] = g;
    color_array[2] = b;
  }
  if(color_state == 2){
    color_array[0] = g;
    color_array[1] = r;
    color_array[2] = b;
  }
  
}
void set_pixel_colors(int cur_LED_vals[]) {

  int i;
  int useColor[3] = {0, 0, 0}; // Instantiate color array

  for (i = 0; i < CNT_LIGHTS; i++) {

    if(cur_LED_vals[i] > 500) {
      get_RGB(get_wave_length(cur_LED_vals[i]), useColor);
      strip.setPixelColor( i, useColor[0], useColor[1], useColor[2] );
    } else {
      strip.setPixelColor( i, 0, 0, 0 );
    }//if
    
  }//for

}

