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

#include "SetAlarmPage.h"
#include "WeatherPage.h"
#include "Util.h"
#include <RTClib.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#define VOLUME_OFFSET   45
#define VOLUME_X_POS    38
#define VOLUME_LENGTH   (240 - 53)

SetAlarmPage::SetAlarmPage(uint8_t id, TimeZone* timeZone, DFRobotDFPlayerMini* mp3) : Page(id) {
  _mp3 = mp3;
  alarmTimeButton.setFont(&FreeSans24pt7b);
  alarmTimeButton.setButtonAlignment(ALIGN_CENTER);
}

void SetAlarmPage::setup(GfxUi* ui) {
  ui->setTextAlignment(CENTER);
  ui->setFont(&FreeSans12pt7b);
  ui->drawString(120, 20, F("Alarm Setup"));

  okButton.init(70, 280, 100, 40, F("OK"));
  alarmTimeButton.init(120, 65, 0, 0,  F("00:00"));
  hourButton.init(45, 85, 47, 40,  F("H"));
  minuteButton.init(105, 85, 40, 40,  F("M"));
  enabledButton.init(170, 85, 40, 40,  F("On"));

  sundayButton.init(9, 143, 27, 27,  F("S"));
  mondayButton.init(41, 143, 27, 27, F("M"));
  tuesdayButton.init(73, 143, 27, 27, F("T"));
  wednesdayButton.init(105, 143, 27, 27, F("W"));
  thursdayButton.init(137, 143, 27, 27, F("T"));
  fridayButton.init(169, 143, 27, 27, F("F"));
  saturdayButton.init(201, 143, 27, 27, F("S"));

  sound1Button.init(9, 202, 27, 27, F("1"));
  sound2Button.init(41, 202, 27, 27, F("2"));
  sound3Button.init(73, 202, 27, 27, F("3"));
  sound4Button.init(105, 202, 27, 27, F("4"));
  sound5Button.init(137, 202, 27, 27, F("5"));
  sound6Button.init(169, 202, 27, 27, F("6"));
  sound7Button.init(201, 202, 27, 27, F("7"));

  ui->setTextAlignment(LEFT);
  ui->setFont(&FreeSans9pt7b);
  ui->drawString(5, sound7Button.X - 9, F("Sounds"));

  drawTime(ui);
  ui->drawButton(hourButton);
  ui->drawButton(minuteButton);

  drawVolume(ui);

  drawEnabledButton(ui, enabledButton, _settings.AlarmEnabled);

  drawToggleButton(ui, sundayButton, _settings.AlarmDays[0]);
  drawToggleButton(ui, mondayButton, _settings.AlarmDays[1]);
  drawToggleButton(ui, tuesdayButton, _settings.AlarmDays[2]);
  drawToggleButton(ui, wednesdayButton, _settings.AlarmDays[3]);
  drawToggleButton(ui, thursdayButton, _settings.AlarmDays[4]);
  drawToggleButton(ui, fridayButton, _settings.AlarmDays[5]);
  drawToggleButton(ui, saturdayButton, _settings.AlarmDays[6]);

  drawSoundButtons(ui);

  ui->drawButton(okButton);
}

uint8_t SetAlarmPage::loop(GfxUi* ui, bool isTouched, TS_Point touchPoint) {
  // Checks whether the screen has been touched
  if (isTouched == true) {
    if (okButton.contains(touchPoint.x, touchPoint.y)) {
      return WEATHER_PAGE;
    }
    else if (enabledButton.contains(touchPoint)) {
      _settings.AlarmEnabled = !_settings.AlarmEnabled;
      drawTime(ui);
      drawEnabledButton(ui, enabledButton, _settings.AlarmEnabled);
    }
    else if (hourButton.contains(touchPoint)) {
      _settings.AlarmTime = _settings.AlarmTime + TimeSpan(3600);
      drawTime(ui);
    }
    else if (minuteButton.contains(touchPoint)) {
      int minute = _settings.AlarmTime.minute() + 1;
      if (minute > 59)
        minute = 0;

      _settings.AlarmTime = DateTime (_settings.AlarmTime.year(), _settings.AlarmTime.month(), _settings.AlarmTime.day(),
                                      _settings.AlarmTime.hour(), minute, 0);
      drawTime(ui);
    }
    else if (sundayButton.contains(touchPoint)) {
      toggleDayButton(ui, sundayButton, 0);
    }
    else if (mondayButton.contains(touchPoint)) {
      toggleDayButton(ui, mondayButton, 1);
    }
    else if (tuesdayButton.contains(touchPoint)) {
      toggleDayButton(ui, tuesdayButton, 2);
    }
    else if (wednesdayButton.contains(touchPoint)) {
      toggleDayButton(ui, wednesdayButton, 3);
    }
    else if (thursdayButton.contains(touchPoint)) {
      toggleDayButton(ui, thursdayButton, 4);
    }
    else if (fridayButton.contains(touchPoint)) {
      toggleDayButton(ui, fridayButton, 5);
    }
    else if (saturdayButton.contains(touchPoint)) {
      toggleDayButton(ui, saturdayButton, 6);
    }
    else if (sound1Button.contains(touchPoint)) {
      setAlarmSound(ui, 1);
    }
    else if (sound2Button.contains(touchPoint)) {
      setAlarmSound(ui, 2);
    }
    else if (sound3Button.contains(touchPoint)) {
      setAlarmSound(ui, 3);
    }
    else if (sound4Button.contains(touchPoint)) {
      setAlarmSound(ui, 4);
    }
    else if (sound5Button.contains(touchPoint)) {
      setAlarmSound(ui, 5);
    }
    else if (sound6Button.contains(touchPoint)) {
      setAlarmSound(ui, 6);
    }
    else if (sound7Button.contains(touchPoint)) {
      setAlarmSound(ui, 7);
    }
    else if (touchPoint.y > sound1Button.Y + VOLUME_OFFSET - 5 && touchPoint.y < sound1Button.Y + VOLUME_OFFSET + 15 + 5)
    {
      int xPos = touchPoint.x - VOLUME_X_POS;
      if (xPos < 0) {
        xPos = 0;
      }

      _settings.AlarmVolume = (int)((xPos / (double)VOLUME_LENGTH) * 100.0);

      if (_settings.AlarmVolume > 100 ) {
        _settings.AlarmVolume = 100;
      }
      else if (_settings.AlarmVolume < 0 ) {
        _settings.AlarmVolume = 0;
      }

      _mp3->volume(_settings.getMp3Volume());
      drawVolume(ui);
    }
  }

  return SET_ALARM_PAGE;
}

void SetAlarmPage::toggleDayButton(GfxUi* ui, Button dayButton, uint8_t day) {
  _settings.AlarmDays[day] = !_settings.AlarmDays[day];
  drawToggleButton(ui, dayButton, _settings.AlarmDays[day]);
}

void SetAlarmPage::drawTime(GfxUi* ui) {
  if (_settings.AlarmEnabled ) {
    alarmTimeButton.setTextColor(ILI9341_GREEN);
  }
  else {
    alarmTimeButton.setTextColor(ILI9341_LIGHTGREY);
  }

  alarmTimeButton.setText(Util::formatTime(_settings.AlarmTime, false, true).c_str());
  ui->drawButton(alarmTimeButton);

  if (_settings.AlarmEnabled ) {
    ui->drawBmp(F("alarmsm.bmp"), 10, alarmTimeButton.Y - 22);
  }
  else {
    ui->getTft()->fillRect(10, alarmTimeButton.Y - 22, 30, 30, ILI9341_BLACK);
  }
}

void SetAlarmPage::drawVolume(GfxUi* ui) {
  int yPos = sound1Button.Y + VOLUME_OFFSET;

  ui->setTextAlignment(LEFT);
  ui->setFont(&FreeSans9pt7b);
  ui->drawString(5, yPos + 12, F("Vol"));

  // clear progress bar
  ui->drawProgressBar(VOLUME_X_POS, yPos, VOLUME_LENGTH, 15, 0, ILI9341_WHITE, ILI9341_BLUE);
  // draw alarm volume progress bar
  ui->drawProgressBar(VOLUME_X_POS, yPos, VOLUME_LENGTH, 15, _settings.AlarmVolume, ILI9341_WHITE, ILI9341_BLUE);
}

void SetAlarmPage::setSettings(Settings settings) {
  _settings = settings;
}

Settings SetAlarmPage::getSettings() {
  return _settings;
}

void SetAlarmPage::setAlarmSound(GfxUi* ui, int8_t value) {
  _settings.AlarmSound = value;
  drawSoundButtons(ui);
  _mp3->stop();
  _mp3->volume((int)((_settings.AlarmVolume / 100.0) * MAX_VOLUME));
  _mp3->play(value);
}

void SetAlarmPage::drawEnabledButton(GfxUi* ui, Button button, bool enabled) {
  button.setFont(&FreeSans9pt7b);
  button.setBackColor(enabled ? ILI9341_WHITE : ILI9341_BLACK);
  button.setTextColor(enabled ? ILI9341_BLACK : ILI9341_WHITE);
  button.setText(enabled ? F("On") : F("Off"));
  ui->drawButton(button);
}

void SetAlarmPage::drawToggleButton(GfxUi* ui, Button button, bool enabled) {
  button.setFont(&FreeSans9pt7b);
  button.setBackColor(enabled ? ILI9341_WHITE : ILI9341_BLACK);
  button.setTextColor(enabled ? ILI9341_BLACK : ILI9341_WHITE);
  ui->drawButton(button);
}

void SetAlarmPage::drawSoundButtons(GfxUi* ui) {
  drawToggleButton(ui, sound1Button, _settings.AlarmSound == 1);
  drawToggleButton(ui, sound2Button, _settings.AlarmSound == 2);
  drawToggleButton(ui, sound3Button, _settings.AlarmSound == 3);
  drawToggleButton(ui, sound4Button, _settings.AlarmSound == 4);
  drawToggleButton(ui, sound5Button, _settings.AlarmSound == 5);
  drawToggleButton(ui, sound6Button, _settings.AlarmSound == 6);
  drawToggleButton(ui, sound7Button, _settings.AlarmSound == 7);
}
