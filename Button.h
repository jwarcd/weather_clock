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

#ifndef _BUTTON_H
#define _BUTTON_H

#include <Arduino.h>
#include "TouchScreen.h"
#include "Control.h"

#include <Fonts/FreeSans18pt7b.h>

#define MAX_BUTTON_TEXT   9
#define MAX_IMAGE_TEXT   13

class Button : public ControlBase {
  public:
    Button();
    Button(int x, int y, int width, int height);
    Button(int x, int y, int width, int height, char* text);
    Button(int x, int y, int width, int height, char* text, char* imageFileName);
    Button(int x, int y, int width, int height, const __FlashStringHelper * text);
    Button(int x, int y, int width, int height, const __FlashStringHelper * text, const __FlashStringHelper * imageFileName);

  public:
    bool Border = true;

  public:
    void init(int x, int y, int width, int height, const __FlashStringHelper * text,  const __FlashStringHelper * imageFileName = NULL);
    virtual ControlType getControlType();
    using ControlBase::contains;
    virtual bool contains(int x, int y);

  public:
    void setText(const char* value);
    void setText(const __FlashStringHelper *value);
    char* getText();
    char* getPreviousText();
    char* getImageFileName();
    void setFont(const GFXfont *f);
    const GFXfont* getFont();
    ButtonAlignment getButtonAlignment();
    void setButtonAlignment(ButtonAlignment alignment);
    TextAlignment getTextAlignment();
    void setTextAlignment(TextAlignment alignment);
    void setTextColor(uint16_t color);
    uint16_t getTextColor();

  private:
    char _imageFileName[MAX_IMAGE_TEXT];
    char _text[MAX_BUTTON_TEXT];
    char _previousText[MAX_BUTTON_TEXT];
    ButtonAlignment _buttonAlignment = ALIGN_XY;
    TextAlignment _textAlignment = CENTER;
    uint16_t _textColor = ILI9341_WHITE;
    const GFXfont* _font = &FreeSans18pt7b;
};

#endif
