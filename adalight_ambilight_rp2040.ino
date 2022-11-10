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
volatile bool isBufferReady = false;
volatile uint8_t magicWord[] = {'A', 'd', 'a'};
volatile uint8_t hi, lo, chk, i;
uint32_t last_time = 0;
volatile uint16_t howManyLED = 0;

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.clear();
  strip.show();
  LED_Demo();
}

void loop() {
  int current_time = millis();
  if (isBufferReady) {
    isBufferReady = false;
    strip.show();
  } else {
    if (current_time - last_time >= 1000) {
      // Send ACK packet every second
      Serial.print("Ada\n");
      last_time = current_time;
    }
  }
}

void LED_Demo() {
  for (int i = 0; i <= 64; i++) {
    strip.rainbow();
    strip.setBrightness(i);
    strip.show();
    delay(25);
  }
  delay(1000);
  for (int i = 64; i > 0; i--) {
    strip.rainbow();
    strip.setBrightness(i);
    strip.show();
    delay(25);
  }
  strip.clear();
  strip.show();
}

inline uint8_t getByteFromSerial() {
  while (!Serial.available()) ;;
  return (uint8_t)Serial.read();
}

void serialEvent() {
  i = 0;
  while (Serial.available()) {
    if (magicWord[i] == (uint8_t)Serial.read()) {
      i++;
      if (i == 3) {
        // Magic packet matched, now buf the header
        // ref: https://www.partsnotincluded.com/visualizing-adalight-header-information/
        hi = getByteFromSerial();
        lo = getByteFromSerial();
        chk = getByteFromSerial();

        if (chk != (hi ^ lo ^ 0x55)) {
          break;
        }
        // Extract metadata from header
        howManyLED = (256L * (long)hi + (long)lo + 1L);
        for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++) {
          byte r, g, b;
          if (i < howManyLED) {
            r = getByteFromSerial();
            g = getByteFromSerial();
            b = getByteFromSerial();
            strip.setPixelColor(i, r, g, b);
          } else {
            strip.setPixelColor(i, 0, 0, 0);
          }
        }
        isBufferReady = true;
        Serial.flush();
      }
    } else {
      strip.clear();
      strip.show();
      Serial.flush();
      break;
    }
  }

}
