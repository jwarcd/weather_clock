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
#include "ArialRoundedMTBold_36.h"
#include "Button.h"

Button::Button() {
}

Button::Button(int x, int y, int width, int height) : Button(x, y, width, height, "") {
}

Button::Button(int x, int y, int width, int height, char* text) : Button(x, y, width, height, text, "") {
}

Button::Button(int x, int y, int width, int height, char* text, char* imageFileName) : ControlBase(x, y, width, height) {
  setText(text);
  _font = &ArialRoundedMTBold_36;
  strncpy(_imageFileName, imageFileName, MAX_IMAGE_TEXT - 1);
}

Button::Button(int x, int y, int width, int height, const __FlashStringHelper * text) : Button(x, y, width, height, text, F("")) {
}

Button::Button(int x, int y, int width, int height, const __FlashStringHelper * text,  const __FlashStringHelper * imageFileName) : ControlBase(x, y, width, height) {
  init(x, y, width, height, text, imageFileName);
}

void Button::init(int x, int y, int width, int height, const __FlashStringHelper * text,  const __FlashStringHelper * imageFileName) {
  ControlBase::init(x, y, width, height);
  setText(text);
  _font = &ArialRoundedMTBold_36;
  if (imageFileName != NULL) {
    PGM_P p = reinterpret_cast<PGM_P>(imageFileName);
    strncpy_P(_imageFileName, (char*)p, MAX_IMAGE_TEXT - 1);
  }
}

ControlType Button::getControlType() { return buton; }

bool Button::contains(int x, int y) {
  int left, right, top, bottom;
    
  if ( _buttonAlignment == ALIGN_CENTER){
    int halfWidth = Width / 2;
    int halfHeight = Height / 2;
    
    left = X - halfWidth;
    right = X + halfWidth;
    top = Y - halfHeight;
    bottom = Y + halfHeight;
  }
  else if ( _buttonAlignment == ALIGN_XY){
    left = X;
    right = X + Width;
    top = Y;
    bottom = Y + Height;
  }
  
  return ( x >= left && x <= right &&  y >= top && y <= bottom );
}

void Button::setText(const char* value){
  strncpy(_previousText, _text, MAX_BUTTON_TEXT - 1);
  strncpy(_text, value, MAX_BUTTON_TEXT - 1);
}

void Button::setText(const __FlashStringHelper *value){
  strncpy(_previousText, _text, MAX_BUTTON_TEXT - 1);
  PGM_P p = reinterpret_cast<PGM_P>(value);
  strncpy_P(_text, (char*)p, MAX_BUTTON_TEXT - 1); 
}

char* Button::getText(){
  return _text;
}

char* Button::getPreviousText(){
  return _previousText;
}

char* Button::getImageFileName(){
  return _imageFileName;
}

void Button::setFont(const GFXfont *font) {
 _font = font;
}
    
const GFXfont* Button::getFont() {
  return _font;
}

ButtonAlignment Button::getButtonAlignment() {
  return _buttonAlignment;
}

void Button::setButtonAlignment(ButtonAlignment alignment) {
  _buttonAlignment = alignment;
}
    
TextAlignment Button::getTextAlignment() {
 return _textAlignment;
}
    
void Button::setTextAlignment(TextAlignment alignment) {
  _textAlignment = alignment;
}

void Button::setTextColor(uint16_t value) {
  _textColor = value;
}
    
uint16_t Button::getTextColor() {
  return _textColor;
}


