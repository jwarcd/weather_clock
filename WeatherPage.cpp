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

#include "WeatherPage.h"
#include "Util.h"
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "ArialRoundedMTBold_14.h"
#include "ArialRoundedMTBold_36.h"
#include <WiFiManager.h>
#include "settings.h"

WeatherPage::WeatherPage(uint8_t id, WeatherClient* weatherClient, TimeZone* timeZone, Settings* settings) : Page(id) {
  _weatherClient = weatherClient;
  _timeZone = timeZone;
  _settings = settings;

  timeButton.setButtonAlignment(ALIGN_CENTER);
  timeButton.setFont(&FreeSans24pt7b);
}

void WeatherPage::setup(GfxUi* ui) {
  lastDate = "";
  lastWifiIcon = "";
  lastAlarmTime = "";
  lastTime = DateTime();
  drawWifiStatus(ui);
  drawCurrentWeather(ui);
  drawTime(ui);
  drawForecast(ui);
  drawAstronomy(ui);
}

uint8_t WeatherPage::loop(GfxUi* ui, bool isTouched, TS_Point touchPoint) {
  // Checks whether the screens has been touched
  if (isTouched == true) {
    // If time was pressed
    if (timeButton.contains(touchPoint)) {
      return SET_ALARM_PAGE;
    }
  }

  long currentMillis = millis();

  // Check if we should update wifi status
  if (lastWifiStatusUpdate == 0 || (currentMillis - lastWifiStatusUpdate > 1000)) {
    drawWifiStatus(ui);
    lastWifiStatusUpdate = currentMillis;
  }

  // Check if we should update weather information
  if (lastConditionsDownload == 0 || (currentMillis - lastConditionsDownload > 1000 * UPDATE_CONDITIONS_INTERVAL_SECS)) {
    updateConditionsData(ui);
    lastConditionsDownload = currentMillis;
  }

  // Check if we should update weather information
  if (lastForcastDownload == 0 || (currentMillis - lastForcastDownload > 1000 * UPDATE_FORCAST_INTERVAL_SECS)) {
    //updateForcastData(ui);
    lastForcastDownload = currentMillis;
  }

  // Check if we should update weather information
  if (lastAstronomyDownload == 0 || (currentMillis - lastAstronomyDownload > 1000 * UPDATE_ASTRONOMY_INTERVAL_SECS)) {
    //updateAstronomyData(ui);
    lastAstronomyDownload = currentMillis;
  }

  drawTime(ui);

  return WEATHER_PAGE;
}

// draws the clock
void WeatherPage::drawWifiStatus(GfxUi* ui) {
  String wifiIcon;
  int rssi = -200;
  if ( WiFi.status() != WL_CONNECTED) {
    wifiIcon = F("nc");
  }
  else {
    rssi = WiFi.RSSI();
    wifiIcon = String(Util::calculateSignalLevel(rssi, 5));
  }

  // Show Wifi RSSI (signal strength) icon
  if (lastWifiIcon != wifiIcon) {
    String wifiIconFile = F("wifi_");
    wifiIconFile += wifiIcon;
    wifiIconFile += F(".bmp");
    ui->drawBmp(wifiIconFile, 220, 0);
    Sprintln(String(F("Wifi RSSI: ")) + String(rssi) + String(F(", Wifi Icon: ")) + wifiIconFile);
    lastWifiIcon = wifiIcon;
  }
}

// Draws the alarm time
void WeatherPage::drawAlarmTime(GfxUi* ui) {
  int bmpYPos = 0;
  int textYPos = bmpYPos + 13;

  // If alarm is enabled, show alarm icon and time for next alarm
  if (_settings->AlarmEnabled) {
    DateTime nowTime = _timeZone->now();

    // Find next alarm day
    int alarmDayOfTheWeek = nowTime.dayOfTheWeek();
    String alarmDay;

    long alarmSeconds = Util::time2long(0, _settings->AlarmTime.hour(), _settings->AlarmTime.minute(), _settings->AlarmTime.second());
    long nowSeconds = Util::time2long(0, nowTime.hour(), nowTime.minute(), nowTime.second());

    if (_settings->AlarmDays[alarmDayOfTheWeek] &&  (alarmSeconds >= nowSeconds)) {
      alarmDay = _timeZone->getDayString(alarmDayOfTheWeek);
    }
    else {
      do {
        alarmDayOfTheWeek++;
        if ( alarmDayOfTheWeek > 6) {
          alarmDayOfTheWeek = 0;
        }
        if (_settings->AlarmDays[alarmDayOfTheWeek]) {
          alarmDay = _timeZone->getDayString(alarmDayOfTheWeek);
          break;
        }
      }
      while ( alarmDayOfTheWeek != nowTime.dayOfTheWeek());
    }

    String alarmTime = alarmDay + F(", ") + Util::formatTime(_settings->AlarmTime, false, true);

    if (alarmTime != lastAlarmTime) {
      ui->drawBmp(F("alarmsm.bmp"), 5, bmpYPos);

      ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      ui->setTextAlignment(LEFT);
      ui->setFont(&ArialRoundedMTBold_14);

      ui->setTextAlignment(LEFT);
      // Erase alarm time
      ui->drawString(30, textYPos, F("pXXXp, pXX:XXp"), true);

      // Draw alarm time
      String alarmTime = alarmDay + F(", ") + Util::formatTime(_settings->AlarmTime, false, true);
      ui->drawString(30, textYPos, alarmDay + F(", ") + Util::formatTime(_settings->AlarmTime, false, true));

      lastAlarmTime = alarmTime;
    }
  }
  else if (lastAlarmTime.length() > 0) {
    // Erase
    ui->getTft()->fillRect(2, bmpYPos, 200, 30, ILI9341_BLACK);
    lastAlarmTime = "";
  }
}

// Draws the current time
void WeatherPage::drawTime(GfxUi* ui) {
  String date = _timeZone->getDateString();
  if (date != lastDate) {
    ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    ui->setTextAlignment(RIGHT);
    ui->setFont(&ArialRoundedMTBold_14);
    ui->drawString(220, 152, F("j XXX, XX XXX p"), true);
    ui->drawString(220, 152, date);
    lastDate = date;
  }

  DateTime nowTime = _timeZone->now();

  if (lastTime.hour() != nowTime.hour() || lastTime.minute() != nowTime.minute()) {
    String timeText = Util::formatTime(nowTime.hour(),  nowTime.minute(), false, false);

    Sprint(F("update time: "));
    Sprintln(timeText);

    timeButton.setText(timeText.c_str());
    ui->drawButton(timeButton);
    lastTime = nowTime;
  }

  drawAlarmTime(ui);
}

// Draws current weather information
void WeatherPage::drawCurrentWeather(GfxUi* ui) {
  // Weather Text
  ui->setFont(&ArialRoundedMTBold_14);
  ui->setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui->setTextAlignment(RIGHT);
  ui->drawString(220, 99, F("XjXXXXXXXXXXXpX"), true);
  ui->drawString(220, 99, _weatherClient->getWeatherText(), false, false);

  ui->setFont(&ArialRoundedMTBold_36);
  ui->setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui->setTextAlignment(RIGHT);
  String degreeSign = F("F");
  if (IS_METRIC) {
    degreeSign = F("C");
  }
  String temp = _weatherClient->getCurrentTemp() + degreeSign;
  ui->drawString(220, 134, F("XXX.XX"), true);
  ui->drawString(220, 134, temp, false, false);

  // Weather Icon
  String weatherIcon = _weatherClient->getTodayIcon();
  ui->drawBmp(weatherIcon + ".bmp", 0, 72);
}

// Draws the three forecast columns
void WeatherPage::drawForecast(GfxUi* ui) {
  drawForecastDetail(ui, 10, 185, 0);
  drawForecastDetail(ui, 95, 185, 2);
  drawForecastDetail(ui, 180, 185, 4);
}

// Helper for the forecast columns
void WeatherPage::drawForecastDetail(GfxUi* ui, uint16_t x, uint16_t y, uint8_t dayIndex) {
  ui->setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui->setFont(&ArialRoundedMTBold_14);
  ui->setTextAlignment(CENTER);
  String day = _weatherClient->getForecastTitle(dayIndex).substring(0, 3);
  day.toUpperCase();
  ui->drawString(x + 25, y, F("XXXX"), true);
  ui->drawString(x + 25, y, day, false, false);

  ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui->drawString(x + 25, y + 14, F("XXX | XXX"), true);
  ui->drawString(x + 25, y + 14, _weatherClient->getForecastHighTemp(dayIndex) + F(" | ") + _weatherClient->getForecastLowTemp(dayIndex), false, false);

  String weatherIcon = _weatherClient->getForecastIcon(dayIndex);
  ui->drawBmp("/mini/" + weatherIcon + ".bmp", x, y + 15);
}

// Draws the moonphase and sunrise/set and moonrise/set
void WeatherPage::drawAstronomy(GfxUi* ui) {
  int moonAgeImage = 24 * _weatherClient->getMoonAge() / 30.0;
  ui->drawBmp("/moon" + String(moonAgeImage) + ".bmp", 120 - 30, 260);

  ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui->setFont(&ArialRoundedMTBold_14);
  ui->setTextAlignment(LEFT);
  ui->setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui->drawString(20, 275, F("Sun"));
  ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui->drawString(15, 290, F("XXX:XXX"), true);
  ui->drawString(15, 290, _weatherClient->getSunriseTime(), false, false);
  ui->drawString(15, 305, F("XXX:XXX"), true);
  ui->drawString(15, 305, _weatherClient->getSunsetTime(), false, false);

  ui->setTextAlignment(RIGHT);
  ui->setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui->drawString(220, 275, F("Moon"));
  ui->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  ui->drawString(220, 290, F("XXX:XXX"), true);
  ui->drawString(220, 290, _weatherClient->getMoonriseTime(), false, false);
  ui->drawString(220, 305, F("XXX:XXX"), true);
  ui->drawString(220, 305, _weatherClient->getMoonsetTime(), false, false);
}

// Update the internet based information and update screen
void WeatherPage::updateConditionsData(GfxUi* ui) {
  Sprintln(F("eatherPage::updateConditionsData"));
  _weatherClient->doUpdate();
  drawCurrentWeather(ui);
  drawForecast(ui);
  drawAstronomy(ui);
}

// Update the internet based information and update screen
void WeatherPage::updateForcastData(GfxUi* ui) {
  Sprintln(F("WeatherPage::updateForcastData"));
  //_weatherClient->updateForecast(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  //drawForecast(ui);
}

// Update the internet based information and update screen
void WeatherPage::updateAstronomyData(GfxUi* ui) {
  Sprintln(F("WeatherPage::updateAstronomyData"));
  // _weatherClient->updateAstronomy(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  //drawAstronomy(ui);
}
