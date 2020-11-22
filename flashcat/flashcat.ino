#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>

// https://learn.adafruit.com/introducing-itsy-bitsy-m0/pinouts
// ItsyBitsy's Dotstar LED is connected to pins 41&40
#define DATAPIN    41  
#define CLOCKPIN   40
Adafruit_DotStar dotstar_led = Adafruit_DotStar(1, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

// There are a number of different NeoPixel rings/jewels you can use, make sure to use
// the correct setting, i.e. NEO_RGBW or NEO_RGB.
#define NEOPIXEL_NUMPIXELS 7
#define NEOPIXEL_PIN 11
Adafruit_NeoPixel pixels(NEOPIXEL_NUMPIXELS, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);

// our RGB -> eye-recognized gamma color
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(9600);
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
    //// You may need to use the commented out line below if the colors seem off.
    //// ... I'm not exactly sure what the line does, it was included in the Adafruit_TCS34725 example.
    //gammatable[i] = 255 - x;
  }
  delay(100);
  getColor();
}

void getColor() {
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on color sensor's LED
  
  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off color sensor's LED

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
  uint32_t g_red = gammatable[(int)red];
  uint32_t g_blue = gammatable[(int)blue];
  uint32_t g_green = gammatable[(int)green];
  uint32_t color = (g_blue<< 16) | (g_red<< 8) | g_green;
  Serial.print("Color:"); Serial.print(color, HEX); Serial.print("\n");
  
  dotstar_led.setPixelColor(0, color);
  dotstar_led.show();
  setNeoPixels(g_red, g_green, g_blue);
}

void setNeoPixels(int red, int green, int blue) {
  pixels.begin();
  pixels.clear();
  for(int i=0; i<NEOPIXEL_NUMPIXELS; i++) {
    // If your colors get mixed up, try changing the order of "green, red, blue" until 
    // your greens are green, and your reds and red.
    pixels.setPixelColor(i, pixels.Color(green, red, blue, 0));
    pixels.show();
  }
}

void loop() {
  // As is, nothing happens in the loop.
  // We find the color when the flashcat is first turned on and emit that.
  delay(1000);
}
