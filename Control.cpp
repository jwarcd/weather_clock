/**The MIT License (MIT)
  Copyright (c) 2017 by Jared Gaillard
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

#include "GfxUi.h"
#include "Control.h"

ControlBase::ControlBase() {
}

ControlBase::ControlBase(int x, int y, int width, int height) {
  init(x, y, width, height);
}

void ControlBase::init(int x, int y, int width, int height) {
  X = x;
  Y = y;
  Width = width;
  Height = height;
}

ControlType ControlBase::getControlType() {
  return unknown;
}

bool ControlBase::contains(int x, int y) {
  int left, right, top, bottom;

  left = X;
  right = X + Width;
  top = Y;
  bottom = Y + Height;

  return ( x >= left && x <= right &&  y >= top && y <= bottom );
}

bool ControlBase::contains(TS_Point point) {
  return contains(point.x, point.y);
}

void ControlBase::setBackColor(uint16_t value) {
  _backColor = value;
}

uint16_t ControlBase::getBackColor() {
  return _backColor;
}
