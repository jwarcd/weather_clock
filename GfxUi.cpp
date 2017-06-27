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

#include <SD.h>
#include "GfxUi.h"
#include "Util.h"

/*
NOTE: Jared Gaillard - This is a modified version of the GfxUi by Daniel Eichhorn.  
*/
GfxUi::GfxUi(Adafruit_ILI9341 *tft) {
  _tft = tft;
}

void GfxUi::clearScreen() {
  _tft->fillScreen(ILI9341_BLACK);
}

void GfxUi::setFont(const GFXfont *f) {
  _tft->setFont(f);
}

Adafruit_ILI9341* GfxUi::getTft() {
  return _tft;
}

void GfxUi::drawString(int x, int y, char *text) {
  drawString(x, y, text, false, true);
}

void GfxUi::drawString(int x, int y, char *text, bool clearOnly, bool eraseBackground) {
  int16_t xAlign, x1, y1;
  uint16_t w, h;
  
  _tft->setTextWrap(false);
  _tft->getTextBounds(text, x, y, &x1, &y1, &w, &h);
  
  //Sprintln("getTextBounds x=" + String(x) + ", y=" + String(y) + ", x1=" + String(x1) + ", y1=" + String(y1) + ", w=" + String(w) + ", h=" + String(h) + ", text=" + text);
  
  switch (_alignment) {
    case LEFT:
      xAlign = x;
      break;
    case CENTER:
      xAlign = x - w/2 - (x1 - x)/2;
      break;
    case RIGHT:
      xAlign = x - w - (x1 - x);
      break;
  }
  
  if (eraseBackground && _textColor != _backgroundColor) {
    _tft->fillRect(xAlign - 1, y - h - 1, w + 2 + (x1 - x), h + 2 + (h -(y - y1)), _backgroundColor);
  }

  if (!clearOnly){
    _tft->setCursor(xAlign, y);
    _tft->print(text);
  }
}

void GfxUi::drawString(int x, int y, String text) {
  drawString(x, y, text, false);
}

void GfxUi::drawString(int x, int y, String text, bool clearOnly, bool eraseBackground) {
  char buf[text.length() + 2];
  text.toCharArray(buf, text.length() + 1);
  drawString(x, y, buf, clearOnly, eraseBackground);
}

void GfxUi::setTextColor(uint16_t c) {
  setTextColor(c, c);
}

void GfxUi::setTextColor(uint16_t c, uint16_t bg) {
  _textColor = c;
  _backgroundColor = bg;
  _tft->setTextColor(_textColor, _backgroundColor);
}

void GfxUi::setTextAlignment(TextAlignment alignment) {
  _alignment = alignment;
}

void GfxUi::drawProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor) {
  if (percentage == 0) {
    _tft->fillRoundRect(x0, y0, w, h, 3, _backgroundColor);
  }
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth = w - 2 * margin;
  _tft->drawRoundRect(x0, y0, w, h, 3, frameColor);
  _tft->fillRect(x0 + margin, y0 + margin, barWidth * percentage / 100.0, barHeight, barColor);
}

// Code mostly from 
// https://github.com/adafruit/Adafruit_ILI9341/blob/master/examples/spitftbitmap/spitftbitmap.ino
// 
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.
//
void  GfxUi::drawBmp(String filename1, int16_t x, int16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= _tft->width()) || (y >= _tft->height())) return;

  char filename[filename1.length() + 2];
  filename1.toCharArray(filename, filename1.length() + 1);

  Sprintln();
  Sprint(F("Loading image '"));
  Sprint(filename);
  Sprintln('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Sprint(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    uint32_t fileSize = read32(bmpFile);
    //Sprint(F("File size: ")); 
    //Sprintln(fileSize);
    
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    //Sprint(F("Image Offset: ")); 
    //Sprintln(bmpImageoffset, DEC);
    
    // Read DIB header
    uint32_t headerSize = read32(bmpFile);
    //Sprint(F("Header size: ")); 
    //Sprintln(headerSize);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      //Sprint(F("Bit Depth: ")); 
      //Sprintln(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        //Sprint(F("Image size: "));
        //Sprint(bmpWidth);
        //Sprint('x');
        //Sprintln(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth  - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if((x2 >= 0) && (y2 >= 0)) { // On screen?
          w = bmpWidth; // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file
          if(x < 0) { // Clip left
            bx1 = -x;
            x   = 0;
            w   = x2 + 1;
          }
          if(y < 0) { // Clip top
            by1 = -y;
            y   = 0;
            h   = y2 + 1;
          }
          if(x2 >= _tft->width())  w = _tft->width()  - x; // Clip right
          if(y2 >= _tft->height()) h = _tft->height() - y; // Clip bottom
  
          // Set TFT address window to clipped image bounds
          _tft->startWrite(); // Requires start/end transaction now
          _tft->setAddrWindow(x, y, w, h);
  
          for (row=0; row<h; row++) { // For each scanline...
  
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + (row + by1) * rowSize;
            pos += bx1 * 3; // Factor in starting column (bx1)
            if(bmpFile.position() != pos) { // Need seek?
              _tft->endWrite(); // End TFT transaction
              bmpFile.seek(pos);
              buffidx = sizeof(sdbuffer); // Force buffer reload
              _tft->startWrite(); // Start new TFT transaction
            }
            for (col=0; col<w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                _tft->endWrite(); // End TFT transaction
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
                _tft->startWrite(); // Start new TFT transaction
              }
              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              _tft->writePixel(_tft->color565(r,g,b));
            } // end pixel
          } // end scanline
          _tft->endWrite(); // End last TFT transaction
        } // end onscreen
        
        Sprint(F("Loaded in "));
        Sprint(millis() - startTime);
        Sprintln(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Sprintln(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t GfxUi::read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t GfxUi::read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void GfxUi::drawButton(Button button){
  setTextAlignment(CENTER);
  setTextColor(button.getTextColor(), button.getBackColor());
  setFont(button.getFont());
  String text = button.getText();

  _tft->setTextWrap(false);
  _tft->setTextSize(1);

  int16_t x, y;
  int16_t x1, y1;
  uint16_t w, h;
  //Sprintln("getTextBounds: buf=" + text + ", x1=" + String(x1) + ",Y1=" + String(y1) + ",w=" + String(w) + ",h=" + String(h));

  if (button.getButtonAlignment() == ALIGN_CENTER){
    String previousText = button.getPreviousText();
    
    if ( previousText.length() > 0 ) {
      drawString(button.X, button.Y, previousText, true);
    }

    if (text.length() > 0){
      drawString(button.X, button.Y, text);
    }
    
    if ( strlen(button.getImageFileName()) > 0 ){
        drawBmp(button.getImageFileName(), button.X, button.Y);   
    }
  }
  else if (button.getButtonAlignment() == ALIGN_XY){
    uint8_t radius = min(button.Width, button.Height) / 5; // Corner radius
    getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    _tft->fillRoundRect(button.X, button.Y, button.Width, button.Height, radius, button.getBackColor());
    
    if (text.length() > 0){
      drawString(button.X + (button.Width / 2) - x1, button.Y + button.Height / 2 + h /2 , text); 
    }
    
    if (strlen(button.getImageFileName()) > 0 ){
        drawBmp(button.getImageFileName(), button.X, button.Y);   
    }
    
    if (button.Border){
      _tft->drawRoundRect(button.X, button.Y, button.Width, button.Height, radius, ILI9341_WHITE);
    }
  }
}

void GfxUi::getTextBounds(String text, int16_t x, int16_t y,
                          int16_t *x1, int16_t *y1, 
                          uint16_t *w, uint16_t *h) {                         
  char buf[text.length() + 2];
  text.toCharArray(buf, text.length() + 1);
  _tft->getTextBounds(buf, x, y, x1, y1, w, h);
}

