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
#define NUMBER_OF_LEDS 511               // 511 is the max supported num. leds
#define LED_DATA_PIN 2                   // Set the correct pin here.
/*****************************************************************************/


#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(NUMBER_OF_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
volatile bool isBufferReady = false;
volatile uint8_t magicWord[] = {'A', 'd', 'a'};
volatile uint8_t hi, lo, chk, i;
uint32_t last_time = 0;
volatile uint16_t howManyLED = 0;
volatile byte r, g, b;
void setup() {
  strip.begin();
  strip.clear();
  strip.show();
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(230400);
  Serial.setTimeout(20);
  //  while (!Serial) {
  //    delay(100);
  //    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //  }
  LED_Demo();
}

void loop() {
  int current_time = millis();
  if (isBufferReady) {
    isBufferReady = false;
    strip.show();
  } else {
    if (current_time - last_time >= 3000) {
      // Send ACK packet every second
      Serial.print("Ada\n");
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      last_time = current_time;
    }
  }
}

void LED_Demo() {
  uint8_t color_pos = 0;
  uint32_t tempColor;
  uint16_t tempPixel;
  strip.setBrightness(100);
  randomSeed(analogRead(0));
  for (uint32_t i = 0; i < NUMBER_OF_LEDS; i++) {
    strip.setPixelColor(i, colorWheel(color_pos));
    strip.show();
    color_pos += 256 / NUMBER_OF_LEDS;
    delayMicroseconds(1500000 / NUMBER_OF_LEDS);
  }
  for (int i = 100; i >= 0; i--) {
    strip.setBrightness(i);
    strip.show();
    delay(20);
  }
  strip.setBrightness(100);
  strip.clear();
  strip.show();
}

void serialEvent() {
  i = 0;
  while (Serial.available()) {
    if (magicWord[i] == (uint8_t)Serial.read()) {
      i++;
      Serial.println("ACKRX");
      if (i == 3) {
        // Magic packet matched, now buf the header
        // ref: https://www.partsnotincluded.com/visualizing-adalight-header-information/

        Serial.println("ACKOK");
        while (!Serial.available()) ;;
        hi = Serial.read();
        while (!Serial.available()) ;;
        lo = Serial.read();
        while (!Serial.available()) ;;
        chk = Serial.read();
        Serial.println("HEDOK");
        // Verify Checksum
        if (chk != (hi ^ lo ^ 0x55)) {
          break;
        }
        // Extract metadata from header
        howManyLED = ((hi << 8) + lo + 1);
        Serial.println("CHKOK");
        Serial.print("NUMLED:"); Serial.println(howManyLED);
        for (uint16_t i = 0; i < NUMBER_OF_LEDS; i++) {
          byte r, g, b;
          if (i < howManyLED) {
            Serial.print("LED:"); Serial.println(i);
            while (!Serial.available());
            r = Serial.read();
            while (!Serial.available());
            g = Serial.read();
            while (!Serial.available());
            b = Serial.read();
            strip.setPixelColor(i, r, g, b);
          } else {
            strip.setPixelColor(i, 0, 0, 0);
          }
        }
        isBufferReady = true;
        Serial.println("ALLOK");
        Serial.flush();
      }
    } else {
      Serial.println("ERR");
      //      strip.clear();
      //      strip.show();
      Serial.flush();
      break;
    }
  }
}

uint32_t colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
