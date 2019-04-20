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

#ifndef _CONTROLBASE_H
#define _CONTROLBASE_H

#include <Arduino.h>
#include "TouchScreen.h"
#include "Constants.h"

enum ControlType {
  unknown,
  buton,
};

class ControlBase {
  public:
    ControlBase();
    ControlBase(int x, int y, int width, int height);
    void init(int x, int y, int width, int height);

  public:
    int X, Y, Width, Height;

  public:
    virtual ControlType getControlType();
    virtual bool contains(int x, int y);
    bool contains(TS_Point point);

  public:
    void setBackColor(uint16_t color);
    uint16_t getBackColor();

  private:
    uint16_t _backColor = ILI9341_BLACK;
};

#endif
