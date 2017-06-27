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
#include "TimeZone.h"
#include "GfxUi.h"
#include "Page.h"
#include "Button.h"
#include "Settings.h"

#include <DFRobotDFPlayerMini.h>

#ifndef _ALARM_PAGE_H
#define _ALARM_PAGE_H

class AlarmPage : public Page {
  public:
    AlarmPage(uint8_t id, DFRobotDFPlayerMini* mp3);
    virtual void setup(GfxUi* ui);
    virtual uint8_t loop(GfxUi* ui, bool isTouched, TS_Point touchPoint);

    void setSettings(Settings value);

  private:
    Settings _settings;
  	DFRobotDFPlayerMini* _mp3;
    long alarmStartTime;
  	long lastVolumeIncrementTime;
  	float currentVolume;
  	float volumeIncrement;
	
  private:
    Button cancelAlarm;
};

#endif

