/**The MIT License (MIT)
Copyright (c) 2015 by Jared Gaillard
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
*/

#include <Arduino.h>
#include <Adafruit_ILI9341.h>   // display library
#include "Settings.h"

#ifdef STMPE610
#include <Adafruit_STMPE610.h>
#else
#include <Adafruit_FT6206.h>
#endif

#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

#ifdef STMPE610
  #define TS_MINX 3800
  #define TS_MAXX 100
  #define TS_MINY 100
  #define TS_MAXY 3750
#else
  #define TS_MINX 240
  #define TS_MAXX 0
  #define TS_MINY 340
  #define TS_MAXY 0
#endif

class TouchScreen {
  public:
    TouchScreen(Adafruit_ILI9341 *tft);

    bool begin();
    bool isTouched();
    TS_Point getTouchPoint();

      private:
    bool checkTouched();

  private:
    Adafruit_ILI9341 *_tft;
  
#ifdef STMPE610
  Adafruit_STMPE610 _touch = Adafruit_STMPE610(STMPE_CS);
#else
  Adafruit_FT6206 _touch = Adafruit_FT6206();
#endif

  TS_Point _touchPoint;
};

#endif


