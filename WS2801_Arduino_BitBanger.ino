#include <Adafruit_WS2801.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#include "command.hpp"

/*****************************************************************************
  Example sketch for driving Adafruit WS2801 pixels!


  Designed specifically to work with the Adafruit RGB Pixels!
  12mm Bullet shape ----> https://www.adafruit.com/products/322
  12mm Flat shape   ----> https://www.adafruit.com/products/738
  36mm Square shape ----> https://www.adafruit.com/products/683

  These pixels use SPI to transmit the color data, and have built in
  high speed PWM drivers for 24 bit color per pixel
  2 pins are required to interface

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

*****************************************************************************/

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t dataPin  = 11;    // Green wire on China PixelStripe ==> MOSI D11
uint8_t clockPin = 13;    // Blue wire on China PixelStripe ==> SCK D13

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. Number of pixels in a row
const int INITIAL_NUM_LED = 1;
//Adafruit_WS2801 strip = Adafruit_WS2801(INITIAL_NUM_LED, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
Adafruit_WS2801 strip = Adafruit_WS2801(INITIAL_NUM_LED);
Command command = Command(&strip);

void setup() {
  Serial.begin(1152000);

  strip.begin();
  strip.show();
  while(!command.IsInitialized()) {
    demo();
    initCommand();
  }
  Serial.print("Init leds: (0x");
  Serial.print(strip.numPixels(), HEX);
  Serial.print(')\n');
  demo();
}

void loop() {
  const int waitTime = 5;
  colorWipe(Color(255, 0, 0), waitTime);
  colorWipe(Color(0, 255, 0), waitTime);
  colorWipe(Color(0, 0, 255), waitTime);
}

void initCommand(void) {
  while(Serial.available() && !command.IsInitialized()) {
    const uint8_t s = Serial.read();// read the incoming char
    command.Init(s);
  }
}

void processCommand(void) {
  while(Serial.available()) {
    const uint8_t s = Serial.read();// read the incoming char
    command.ProcessCommand(s);
  }
}

static uint16_t rainbowCyclePos = 0;
void demo() {
  const int waitTime = 0;
  rainbowCyclePos++;
  if(rainbowCyclePos >= 256*5) {
    rainbowCyclePos = 0;
  }
  rainbowCycle(rainbowCyclePos);
}

void rainbow(uint8_t j) {
  int i;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel( (i + j) % 255));
  }
  strip.show();   // write all the pixels out
}

// Slightly different, this one makes the rainbow wheel equally distributed
// along the chain
void rainbowCycle(uint16_t j) {
  int i;
  for (i = 0; i < strip.numPixels(); i++) {
    // tricky math! we use each pixel as a fraction of the full 96-color wheel
    // (thats the i / strip.numPixels() part)
    // Then add in j which makes the colors go around per pixel
    // the % 96 is to make the wheel cycle around
    strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
  }
  strip.show();   // write all the pixels out
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

/* Helper functions */

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

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
