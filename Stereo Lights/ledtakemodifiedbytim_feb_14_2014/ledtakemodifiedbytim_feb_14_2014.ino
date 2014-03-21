
//Jeremy Blum's Arduino Tutorial Series - Episode 14 - Lights and Sound Holiday Special
//Sample Code 2 - Driving all 50 LEDs using an array powered by both stereo channels
//http://www.jeremyblum.com
//WS2801 Library and Helper Functions by Adafruit http://www.adafruit.com

// John Basila's auto level adjustments
// Songs in most cases can have different maximum peak level.
// The code will set the maximum value of all read value samples
// and allow the map() API to map the exact range of input to the
// corresponding color range which is 0-255.
//
// A decay of the maximum value is also done in this code by decrementing
// the maximum value by DECAY_BY so that the input mapping will be
// automatically adjusted when lower input samples are used.
//
// There is no need for the constraint() api, as the maximum value will
// make sure that the map() api will never slide above the mapped level
// thanks to the maximum value checking.
//
// There are also cases where a song is rather too much active and causes
// the sampling to never read a low level. This is also handled by moving
// the lowest value (which is 0) up by DECAY_BY while checking if a new
// minimum was sampled making it the new minimum.
//
// Except for the SAMPELING_INTERVAL, there is no need to change of these
// values
#define STARTING_LEVEL     255
#define DECAY_BY           5
#define MINIMUM_LEVEL      3
#define SAMPELING_INTERVAL 20

// When using a mode that has a dimming ability, then the fading will be done
// by a percentage of FADE_BY. For example, if FADE_BY is 10, then the fading
// will be done by 10% for each SAMPELING_INTERVAL.
#define FADE_BY            5

// For cycling between display patterns, this define sets the interval between
// each display mode. For example, if CYCLE_DM is set to 10000, then after
// 10000 mills seconds (10 seconds) the next patter will be used.
#define CYCLE_DM           0

// Setting the starting patter to display. If CYCLE_DM is zero, then this display mode
// will stay forever.
byte g_displayMode = 0;

typedef void (*displayMode_init)();
typedef void (*displayMode)();

struct
{
  displayMode_init initFunction;
  displayMode samplerFunction;
} g_displayModes[] =
{
  { displayModeWave_init, displayModeWave },
  { displayModePulse_init, displayModePulse },
  { displayModeFade_init, displayModeFade },
  { displayModeSpectrum_init, displayModeSpectrum },
  { displayModeSpectrum_init, displayModeSpectrumRandom }
};

/* 
 The circuit:
 * A0 from Amplififed Left Channel
 * A1 from Amplified Right Channel  
 * Digital 2 to Light Clock Line
 * Digital 3 to Light Data Line
 * Don't forget to connect all your grounds!
 */

//SPI Library
#include <SPI.h>

//RGB LED Library
//You can download it here: https://github.com/adafruit/Adafruit-WS2801-Library
#include <Adafruit_WS2801.h>

//Analog Input Pins
const int left_channel = 0;
const int right_channel = 1;

//Light Control Pins
const int light_data = 3;
const int light_clk = 2;

//Set Strip Constants
const int length = 50;
const int half = length/2;

//Library Setup
Adafruit_WS2801 strip = Adafruit_WS2801(length, light_data, light_clk);

struct
{
  byte hue;
  byte intensity;
} g_leftArray[half], g_rightArray[half];


/********************************************************************
 * Following are functions that help calculate the color of a pixel *
 ********************************************************************/

/* Helper functions */
//http://www.ladyada.net/products/rgbledpixel/

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

// The value of number in intensity percentage
byte percentageCalculator(byte number, byte intensity)
{
  unsigned int bReturn = number;
  bReturn = (bReturn * intensity) / 100;
  return bReturn;
}

//Input a value 0 to 255 to get a color value.
//The colors are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos, byte intensity)
{
  byte red = 0;
  byte green = 0;
  byte blue = 0;

  if (WheelPos)
  {
    if (WheelPos < 85)
    {
      red = 255 - WheelPos * 3;
      blue = WheelPos * 3;
    } 
    else if (WheelPos < 170)
    {
      WheelPos -= 85;
      green = WheelPos * 3;
      blue = 255 - WheelPos * 3;
    } 
    else
    {
      WheelPos -= 170; 
      red = WheelPos * 3;
      green = 255 - WheelPos * 3;
    }
  }

  red = percentageCalculator(red, intensity);
  green = percentageCalculator(green, intensity);
  blue = percentageCalculator(blue, intensity);

  return Color(red, green, blue);
}

/********************************************************************
 * After processing the input and setting the colors for each pixel *
 * in the arrays, the following function set the color of the pixels*
 * in the strip and display them.                                   *
 ********************************************************************/
void displayLeds()
{
  //Go through each Pixel on the strip and set its color
  for (int i = 0; i < half; ++i)
  {
    //set pixel color
    strip.setPixelColor(i, Wheel(g_leftArray[i].hue, g_leftArray[i].intensity));
    strip.setPixelColor(length-i-1, Wheel(g_rightArray[i].hue, g_rightArray[i].intensity));
  }

  //Display the new values
  strip.show();
}

/********************************************************************
 * The following functions are responsible for retrieving the       *
 * sample from the input channel and calculate the maximum value    *
 * and minimum value of the inputs                                  *
 ********************************************************************/
byte readChannel(const int channelPin, int& channelMaxValue, int& channelMinValue)
{
  byte bReturnValue = 0;
  int channelValue = 0;

  // Read the channel value
  channelValue = analogRead(channelPin);

  // Check and set a new maximum value if the read value is greater
  // than the maximum value.
  channelMaxValue = max(channelMaxValue, channelValue);

  // Check and set the lower level
  channelMinValue = min(channelMinValue, channelValue);

  // Map the read value to a color value
  bReturnValue = map(channelValue, channelMinValue, channelMaxValue, 0, 255);

  // Decay
  //
  // The decay is required to allow the maximum value to decay down
  // to the lower value which is MINIMUM_LEVEL to insure an automatic
  // adjustment of the scale of input data read for lower level inputs.
  channelMaxValue -= DECAY_BY;
  channelMaxValue = max(channelMaxValue, MINIMUM_LEVEL);

  // Also, for the lower limit, it will be pushed up by DECAY_BY
  // while the input value will override it if it is less the current
  // one.
  channelMinValue += DECAY_BY;
  channelMinValue = min(channelMinValue, channelMaxValue - 1);

  return bReturnValue;
}

byte hueLeft()
{
  static int leftChannelMaxValue = STARTING_LEVEL;
  static int leftChannelMinValue = 0;
  return readChannel(left_channel, leftChannelMaxValue, leftChannelMinValue);
  
}

byte hueRight()
{
  static int rightChannelMaxValue = STARTING_LEVEL;
  static int rightChannelMinValue = 0;
  return readChannel(right_channel, rightChannelMaxValue, rightChannelMinValue);
}

/********************************************************************
 * From this point, all functions are related to the different      *
 * display methods of the strip.                                    *
 * The following functions will set the color (hue) and intensity   *
 * of each pixel in the array.                                      *
 * Each algorithm is made up of two functions, one for initializing *
 * the array for the left and right channel and the second is the   *
 * algorithm itself (the display function).                         *
 ********************************************************************/

///////////////////////////////////////////////////////////////////////
// Original algorithm made by Jeremy Blum. It starts from the middle of
// of the array and spreads to the sides
///////////////////////////////////////////////////////////////////////
void displayModeWave_init()
{
  for(int i = 0; i < half; ++i)
  {
    g_leftArray[i].intensity = 100;
    g_rightArray[i].intensity = 100;
  }
}

void displayModeWave()
{
  //Shift the current values.
  for (int i = 0; i < half - 1; ++i)
  {
    g_leftArray[i].hue = g_leftArray[i + 1].hue;
    g_rightArray[i].hue = g_rightArray[i + 1].hue;
  }

  //Fill in the new value at the end of each array
  //Set the hue (0-255) and 24-bit color depending on left channel value
  g_leftArray[half-1].hue = hueLeft();
  //Set the hue (0-255) and 24-bit color depending on right channel value
  g_rightArray[half-1].hue = hueRight();
}

///////////////////////////////////////////////////////////////////////
// This is a very simple algorithm, it will color all pixels with the 
// same color. The color is based on the input.
///////////////////////////////////////////////////////////////////////
void displayModePulse_init()
{
  displayModeWave_init();
}

void displayModePulse()
{
  byte leftHue = hueLeft();
  byte rightHue = hueRight();

  for(int i = 0; i < half; ++i)
  {
    g_leftArray[i].hue = leftHue;
    g_rightArray[i].hue = rightHue;
  }
}

///////////////////////////////////////////////////////////////////////
// This is the same as the Wave display by Jeremy Blum except that
// it fades the light as it goes through to the sides
///////////////////////////////////////////////////////////////////////
void displayModeFade_init()
{
  for (int i = 0; i < half; ++i)
  {
    byte fadeBy = 100 / half;
    g_leftArray[i].intensity = 100 - fadeBy * (half - i - 1);
    g_rightArray[i].intensity = 100 - fadeBy * (half - i - 1);
  }
}

void displayModeFade()
{
  displayModeWave();
}

///////////////////////////////////////////////////////////////////////
// The following are two algorithms that are almost the same except
// for the color choosing. The first will chose a color based on the
// location of the pixel, which results in a rainbow like strip that
// will light a pixel based on the input and fades away. The location
// of the pixel that is lit is based on the input sample, if the
// level of the sample is high, then the farthest pixel will be lit
// and if is low then the pixel closest to the center will be lit.
// This algorithm will give a very nice effect of a spectrum and a
// nice effect of "dancing" with the music.
//
// The second algorithm is exactly the same, except that the colors
// are chosen randomly.
///////////////////////////////////////////////////////////////////////
void displayModeSpectrum_init()
{
}

void displayModeSpectrum(byte leftChannelHue, byte righChannelHue)
{
  for(int i = 0; i < half; ++i)
  {
    g_leftArray[i].intensity = g_leftArray[i].intensity > FADE_BY ? g_leftArray[i].intensity - FADE_BY : 0;
    g_leftArray[i].intensity = max(g_leftArray[i].intensity, 5);
    g_rightArray[i].intensity = g_rightArray[i].intensity > FADE_BY ? g_rightArray[i].intensity - FADE_BY : 0;
    g_rightArray[i].intensity = max(g_rightArray[i].intensity, 5);
  }

  byte leftHue = hueLeft();
  if(leftHue)
  {
    byte leftIndex = map(leftHue, 0, 255, 1, half);
    g_leftArray[half - leftIndex].hue = leftChannelHue ? leftChannelHue : leftHue;
    g_leftArray[half - leftIndex].intensity = 100;
  }

  byte rightHue = hueRight();
  if(rightHue)
  {
    byte rightIndex = map(rightHue, 0, 255, 1, half);
    g_rightArray[half - rightIndex].hue = righChannelHue ? righChannelHue : rightHue;
    g_rightArray[half - rightIndex].intensity = 100;
  }
}

void displayModeSpectrum()
{
  displayModeSpectrum(0, 0);
}

void displayModeSpectrumRandom()
{
  displayModeSpectrum(random(1, 256), random(1, 256));
}

const byte g_numberOfDisplayModes = sizeof(g_displayModes)/sizeof(g_displayModes[0]);

void setup()
{
  if( g_displayMode < g_numberOfDisplayModes )
    g_displayModes[g_displayMode].initFunction();
Serial.begin(9600);
  //Initialize Strip
  strip.begin();
  strip.show();
}

unsigned long nextModeSwitch = millis() + CYCLE_DM;

void loop()
{
  if( g_displayMode < g_numberOfDisplayModes )
  {
    Serial.println(analogRead(2));
    if( CYCLE_DM )
    {
      unsigned long now = millis();

      if(now >= nextModeSwitch)
      {
        g_displayMode = (g_displayMode + 1) % g_numberOfDisplayModes;
        nextModeSwitch += CYCLE_DM;
        g_displayModes[g_displayMode].initFunction();
      }
    }

    g_displayModes[g_displayMode].samplerFunction();

    // read the left and right array channels, calculating the color based
    // on their hue and intensity and display them in the led strip.
    displayLeds();

    // sample delay
    delay(SAMPELING_INTERVAL);
  }
}

