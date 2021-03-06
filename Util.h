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

#pragma once

#ifndef _UTIL_H
#define _UTIL_H

#include <Arduino.h>
#include <RTClib.h>

#define DEBUG


#ifdef DEBUG
#define Sprintln(a) (Serial.println(a))
#define Sprint(a) (Serial.print(a))
#else
#define Sprintln(a)
#define Sprint(a)
#endif

class Util {

  public:
    static int calculateSignalLevel(int rssi, int numLevels);

    static String getTime(time_t *timestamp);
    static String formatTime(DateTime dateTime, bool hour24, bool ampm);
    static String formatTime(int hour, int minute, int second);
    static String formatTime(int hour, int minute, bool hour24, bool ampm);
    static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s);

    static int I2C_ClearBus();
    static void clearBusI2C();
};

#endif
