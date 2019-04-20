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

#ifndef _WEATHER_PAGE_H
#define _WEATHER_PAGE_H

#include <Arduino.h>
#include "WeatherClient.h"
#include "TimeZone.h"
#include "GfxUi.h"
#include "Page.h"
#include "Button.h"

class WeatherPage : public Page {
  public:
    WeatherPage(uint8_t id, WeatherClient* weatherClient, TimeZone* timeZone, Settings* settings);
    virtual void setup(GfxUi* ui);
    virtual uint8_t loop(GfxUi* ui, bool isTouched, TS_Point touchPoint);

  private:
    void drawWifiStatus(GfxUi* ui);
    void drawTime(GfxUi* ui);
    void drawAlarmTime(GfxUi* ui);
    void drawCurrentWeather(GfxUi* ui);
    void drawForecast(GfxUi* ui);
    void drawForecastDetail(GfxUi* ui, uint16_t x, uint16_t y, uint8_t dayIndex);
    void drawAstronomy(GfxUi* ui);

  private:
    void updateConditionsData(GfxUi* ui);
    void updateForcastData(GfxUi* ui);
    void updateAstronomyData(GfxUi* ui);
    void drawSeparator(GfxUi* ui, uint16_t y);

  private:
    long lastConditionsDownload = 0;
    long lastForcastDownload = 0;
    long lastAstronomyDownload = 0;
    long lastWifiStatusUpdate = 0;
    WeatherClient* _weatherClient;
    TimeZone* _timeZone;
    Settings* _settings;
    String lastDate;
    DateTime lastTime;
    String lastAlarmTime;
    String lastWifiIcon = "";
    Button timeButton = Button(120, 63, 240, 36);
};

#endif
