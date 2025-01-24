/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software witout restriction, including without limitation the rights
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
*/

#include "ssd1306.h"

typedef struct {
  const char *name;
  void (*run)(void);
  uint32_t result;
} TestInfo;

void benchmark(TestInfo *test) {
  uint32_t start = micros();
  uint32_t end;
  for (uint8_t i = 0; i < 8; i++) {
    test->run();
  }
  end = micros();
  test->result = ((uint32_t)(end - start)) / 8;
}

const char clearScreen[]  PROGMEM = "clearScreen";
const char drawRect[]     PROGMEM = "drawRect";
const char sendBuffer[]   PROGMEM = "sendBuffer";
const char drawText[]     PROGMEM = "drawText";
const char drawText2x[]   PROGMEM = "drawText 2x";

void doClearScreen() {
  ssd1306_clearScreen();
}

void doSendBuffer() {
}

void drawRect32x32() {
  ssd1306_drawRect(16, 16, 48, 48);
}

void drawTextHello() {
  ssd1306_printFixed(0, 0, "Hello, SSD1306_DIRECT_TEST!", STYLE_NORMAL);
}

void drawTextHello2x() {
  ssd1306_printFixedN(0, 0, "Hello, world!", STYLE_NORMAL, FONT_SIZE_2X);
}

TestInfo tests[] = {
  {
    .name = clearScreen,
    .run = doClearScreen,
  },
  {
    .name = sendBuffer,
    .run = doSendBuffer,
  },
  {
    .name = drawRect,
    .run = drawRect32x32,
  },
  {
    .name = drawText2x,
    .run = drawTextHello2x,
  },
  {
    .name = drawText,
    .run = drawTextHello,
  },
};

void initLcd() {
  ssd1306_128x64_i2c_init();
  ssd1306_setContrast(100);
  ssd1306_invertMode();
  // ssd1306_normalMode();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
}

void setup() {
  Serial.begin(115200);
  initLcd();
  for (uint8_t i = 0; i < sizeof(tests) / sizeof(TestInfo); i++) {
    ssd1306_clearScreen();
    benchmark(&tests[i]);
    const char *p = tests[i].name;
    while (pgm_read_byte(p)) {
      Serial.print((char)pgm_read_byte(p++));
    }
    Serial.print(": ");
    Serial.print(tests[i].result);
    Serial.println("us");
    doSendBuffer();
    delay(2000);
  }
  doSendBuffer();
}

void loop() {
}
