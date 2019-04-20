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

#include "TouchScreen.h"
#include "Settings.h"

TouchScreen::TouchScreen(Adafruit_ILI9341 *tft) {
  _tft = tft;
}

bool TouchScreen::begin(){
  bool result = _touch.begin();

#ifdef STMPE610
  // we'll use STMPE's GPIO 2 for backlight control610
  _touch.writeRegister8(STMPE_GPIO_DIR, _BV(2));
  _touch.writeRegister8(STMPE_GPIO_ALT_FUNCT, _BV(2));
  // backlight on
  _touch.writeRegister8(STMPE_GPIO_SET_PIN, _BV(2));
#endif

  return result;
}

bool TouchScreen::isTouched(){
  bool touched = _touch.touched();
  if (touched) {
    _touchPoint = _touch.getPoint();
    
    while (checkTouched()) {
      _touchPoint = _touch.getPoint();  
    }

    //Sprintln("isTouched1 x=" + String(_touchPoint.x) + ", y=" + String(_touchPoint.y));

    _touchPoint.x = map(_touchPoint.x, TS_MINX, TS_MAXX, 0, _tft->width());
    _touchPoint.y = map(_touchPoint.y, TS_MINY, TS_MAXY, 0, _tft->height());

    //Sprintln("isTouched2 x=" + String(_touchPoint.x) + ", y=" + String(_touchPoint.y));
  }

  return touched;
}

TS_Point TouchScreen::getTouchPoint(){
  return _touchPoint;
}

bool TouchScreen::checkTouched(){
#ifdef STMPE610
  return !_touch.bufferEmpty() || _touch.touched();
#else
  return  _touch.touched();
#endif
}
