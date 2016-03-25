#include "Arduino.h"

void getAudiomsg(){

  // Reset EQ7 chip
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);
  
  // Change color mode if stomp button is pressed
  if(stomp_pressed()) {
    change_color_mode();
    delay(100); // Keeps the pedal from switching colors too quickly
  }

  // Get the sum of the amplitudes of all 7 frequency bands
  amp_sum_L = get_freq_sum(ANALOG_PIN_L);
  amp_sum_R = get_freq_sum(ANALOG_PIN_R);

  /* If monomode is active, make both L and R equal to the
     value of L */
  if(monomode) amp_sum_L = amp_sum_R;
  
  setsensitivity();
  
  
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

void setsensitivity(){
  /*  SENSITIVITY_DIVISOR lowers the range of the possible pot values
   *  If the value is higher than the max or lower than the min,
   *  set it to the max or min respectively.
   */
  sensitivity = (analogRead(SENSITIVITY_POT_PIN) / SENSITIVITY_DIVISOR) * SENSITIVITY_MULTIPLIER;
  max_amplitude = MAX_AMPLITUDE - (((1023 - analogRead(SENSITIVITY_POT_PIN)) / SENSITIVITY_DIVISOR) * MAX_AMPLITUDE_MULTIPLIER);

}

