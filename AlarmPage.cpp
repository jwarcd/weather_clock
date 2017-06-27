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

#include "AlarmPage.h"
#include "settings.h"
#include "Util.h"

AlarmPage::AlarmPage(uint8_t id, DFRobotDFPlayerMini* mp3) : Page(id) {
  _mp3 = mp3;
  cancelAlarm.setButtonAlignment(ALIGN_XY);
  cancelAlarm.Border = false;
}

void AlarmPage::setup(GfxUi* ui) {
  ui->setTextAlignment(CENTER);
  ui->setFont(&FreeSans18pt7b);
  
  // Draw the alarm time at the top
  ui->drawString(120, 35, Util::formatTime(_settings.AlarmTime, false, true));

  // Initialize the cancel alarm image button
  cancelAlarm.init(10, 50, 220, 270, F(""), F("alarmlg.bmp"));
  ui->drawButton(cancelAlarm);

  alarmStartTime = millis();
  lastVolumeIncrementTime = alarmStartTime;
  volumeIncrement = _settings.getMp3Volume() * 0.1;   // 10 steps
  currentVolume = volumeIncrement;
  _mp3->volume((int)currentVolume);
}

uint8_t AlarmPage::loop(GfxUi* ui, bool isTouched, TS_Point touchPoint) {
  // Checks whether the screen has been touched
  if (isTouched == true) {
    // If cancel button was pressed
    if (cancelAlarm.contains(touchPoint)) {
      return WEATHER_PAGE;
    }
  }
  
  long currentTime = millis();

  // Turn off alarm after 5 minutes
  if ((currentTime - alarmStartTime) > (1000 * ALARM_ON_MAX_SECONDS)) { 
    // Alarm on for 5 minutes, canceling
    return WEATHER_PAGE;  
  }
  
  // Gradually increase alarm volume every two seconds up to user's set value
  uint8_t userSetVolumeLevel = _settings.getMp3Volume();
  if (currentVolume < userSetVolumeLevel && ((currentTime - lastVolumeIncrementTime) > 2000)) { 
	  currentVolume += volumeIncrement;
	  
    if ( currentVolume > userSetVolumeLevel) {
      currentVolume = userSetVolumeLevel;
	  }

    Sprintln("Set Volume: " + String(currentVolume));
	  _mp3->volume((int)currentVolume);
    lastVolumeIncrementTime = currentTime;  
  }
  
  return ALARM_ACTIVE_PAGE;
}

void AlarmPage::setSettings(Settings settings){
  _settings = settings;
}



