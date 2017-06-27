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
#include <RTClib.h>
#include "Settings.h"

#ifndef _TIMEZONE_H
#define _TIMEZONE_H

#define UPDATE_TIME_INTERVAL_SECS  60 * 60 // Update every hour
#define WIFI_DELAY        500
// Max SSID octets.
#define MAX_SSID_LEN      32
// Wait this much until device gets IP.
#define MAX_CONNECT_TIME  30000

class TimeZone {
  public:
    TimeZone(Settings* settings);

  public:
    void begin();
	  DateTime now();
	  void adjust(DateTime value);
	  void updateTimeData();
    String getDateString();
    String getDayString(uint8_t dayOfWeek);
    String getMonthString(uint8_t month);
   
  private:
    String httpsPost(const char* host, String url, String contentType, String data, int &errorCode);
    String httpsGet(const char* host, String url,  int &errorCode);
    void scanAndLocateTimeZone();
    bool getLatLong(double &latitude, double &longitude);
    bool getTimeZone();
    String getWifiAccessPointJson();

  private:
    Settings* _settings;
    RTC_DS3231 _rtc;
    long lastTimeUpdate = 0;
    char ssid[MAX_SSID_LEN];
    bool ntpSyncEventTriggered;
};

#endif

