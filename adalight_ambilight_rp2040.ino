/******************************************************************************
  Copyright (c) 2022 Shreyas Kulkarni
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
******************************************************************************/

/********************************* SETTINGS **********************************/
#define NUMBER_OF_LEDS (64U)
#define LED_DATA_PIN (2U)
/*****************************************************************************/


#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(NUMBER_OF_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.clear();
  strip.show();
  LED_Demo();
}

void loop() {

}

void LED_Demo(){
  for(int i = 0; i<=64; i++){
    strip.rainbow();
    strip.setBrightness(i);
    strip.show();
    delay(25);
  }
  delay(1000);
  for(int i = 64; i>0; i--){
    strip.rainbow();
    strip.setBrightness(i);
    strip.show();
    delay(25);
  }
  strip.clear();
  strip.show();
}
