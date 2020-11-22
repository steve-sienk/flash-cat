#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>


#define NUMPIXELS 1 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    41  
#define CLOCKPIN   40
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

#define NEOPIXEL_NUMPIXELS 7
#define NEOPIXEL_PIN 11
Adafruit_NeoPixel pixels(NEOPIXEL_NUMPIXELS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
  //Serial.println("Color View Test!");

  if (tcs.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;


    gammatable[i] = x;
//     gammatable[i] = 255 - x;
    //Serial.println(gammatable[i]);
  }
  delay(100);
  getColor();
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to 
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.
void getColor() {
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED
  
  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
  uint32_t g_red = gammatable[(int)red];
  uint32_t g_blue = gammatable[(int)blue];
  uint32_t g_green = gammatable[(int)green];
  uint32_t color = (g_blue<< 16) | (g_red<< 8) | g_green;
  Serial.print("Color:"); Serial.print(color, HEX); Serial.print("\n");
  
  strip.setPixelColor(0, color); // 'On' pixel at head
  strip.show();
  setNeoPixels(g_red, g_green, g_blue);
}

void setNeoPixels(int red, int green, int blue) {
  pixels.begin();
  pixels.clear();
  for(int i=0; i<NEOPIXEL_NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(green, red, blue, 0));
    pixels.show();
  }
}

void loop() {
  delay(1000);
}
