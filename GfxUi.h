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

#pragma once

#ifndef _GFX_UI_H
#define _GFX_UI_H

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SD.h>
#include "Button.h"
#include "Control.h"

#include <Adafruit_ImageReader.h> // Image-reading functions

#define BUFFPIXEL 20

/*
  NOTE: Jared Gaillard - This is a modified version of the GfxUi by Daniel Eichhorn.
*/
class GfxUi {
  public:
    GfxUi(Adafruit_ILI9341 *tft);
    void drawString(int x, int y, char *text);
    void drawString(int x, int y, String text);
    void drawString(int x, int y, char *text, bool clearOnly, bool eraseBackground = true);
    void drawString(int x, int y, String text, bool clearOnly, bool eraseBackground = true);
    void setTextAlignment(TextAlignment alignment);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void drawBmp(String filename, int16_t x, int16_t y);
    void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor);
    void setFont(const GFXfont *f = NULL);
    Adafruit_ILI9341* getTft();
    void clearScreen();

  public:
    //void drawControl(ControlBase control);
    void drawButton(Button button);
    void getTextBounds(String text, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1,
                       uint16_t *w, uint16_t *h);
  private:
    void clearText(int x, int y, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t textColor);

  private:
    Adafruit_ILI9341 *_tft;
    Adafruit_ImageReader reader;
    TextAlignment _alignment = LEFT;
    uint16_t _textColor;
    uint16_t _backgroundColor;
    uint16_t read16(File &f);
    uint32_t read32(File &f);
};

#endif
