#include <SPI.h>

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

RF24 radio(9, 10);

const uint8_t num_channels = 128;
uint8_t values[num_channels];

void setup(void) {
  //
  // Print preamble
  //

  Serial.begin(57600);
  //printf_begin();
  Serial.print("\n\rRF24/examples/scanner/\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setAutoAck(false);

  // Get into standby mode
  radio.startListening();
  radio.stopListening();

  // Print out header, high then low digit
  int i = 0;
  while ( i < num_channels )
  {
    Serial.print(i>>4, HEX);
    ++i;
  }
  Serial.println();
  i = 0;
  while ( i < num_channels )
  {
    Serial.print(i&0xf, HEX);
    ++i;
  }
  Serial.println();
}

//
// Loop
//

const int num_reps = 10;

void loop(void)
{
  // Clear measurement values
  memset(values,0,sizeof(values));

  // Scan all channels num_reps times
  int rep_counter = num_reps;
  while (rep_counter--)
  {
    int i = num_channels;
    while (i--)
    {
      // Select this channel
      radio.setChannel(i);

      // Listen for a little
      radio.startListening();
      delayMicroseconds(128);
      radio.stopListening();

      // Did we get a carrier?
      if ( radio.testCarrier() )
        ++values[i];
    }
  }

  // Print out channel measurements, clamped to a single hex digit
  int i = 0;
  while ( i < num_channels )
  {
    Serial.print(min(0xf,values[i]&0xf), HEX);
    ++i;
  }
  Serial.println();
}

