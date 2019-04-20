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

#ifndef _SET_ALARM_PAGE_H
#define _SET_ALARM_PAGE_H

#include <Arduino.h>
#include "TimeZone.h"
#include "GfxUi.h"
#include "Page.h"
#include "Button.h"
#include "Settings.h"

#include <DFRobotDFPlayerMini.h>

class SetAlarmPage : public Page {
  public:
    SetAlarmPage(uint8_t id, TimeZone* timeZone, DFRobotDFPlayerMini* mp3);
    virtual void setup(GfxUi* ui);
    virtual uint8_t loop(GfxUi* ui, bool isTouched, TS_Point touchPoint);

    void setSettings(Settings value);
    Settings getSettings();

  private:
    void setAlarmSound(GfxUi* ui, int8_t value);
    void drawTime(GfxUi* ui);
    void drawVolume(GfxUi* ui);
    void drawToggleButton(GfxUi* ui, Button button, bool enabled);
    void drawEnabledButton(GfxUi* ui, Button button, bool enabled);
    void drawSoundButtons(GfxUi* ui);
    void toggleDayButton(GfxUi* ui, Button dayButton, uint8_t day);

  private:
    Settings _settings;
    DFRobotDFPlayerMini* _mp3;

    Button okButton;
    Button alarmTimeButton;
    Button hourButton;
    Button minuteButton;
    Button enabledButton;

    Button mondayButton;
    Button tuesdayButton;
    Button wednesdayButton;
    Button thursdayButton;
    Button fridayButton;
    Button saturdayButton;
    Button sundayButton;

    Button sound1Button;
    Button sound2Button;
    Button sound3Button;
    Button sound4Button;
    Button sound5Button;
    Button sound6Button;
    Button sound7Button;
};

#endif
