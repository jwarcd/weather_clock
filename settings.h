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

#pragma once

#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <RTClib.h>

#define DEBUG

// Define for the STMPE610 2.4" resistive touch display, comment out for the 2.8 capacitive touch FT6206. See TouchScreen.h
#define STMPE610

// HOSTNAME for OTA update
#define OTA_HOSTNAME                        "WEATHER-CLOCK-"
#define OTA_PASSWORD                        "XXXXXXXXXXXXXX"

// Setup
#define UPDATE_CONDITIONS_INTERVAL_SECS     10 * 60           // Update every 10 minutes
#define UPDATE_FORCAST_INTERVAL_SECS        30 * 60           // Update every 30 minutes 
#define UPDATE_ASTRONOMY_INTERVAL_SECS      120 * 60          // Update every 2 hours 
#define ALARM_ON_MAX_SECONDS                5 * 60            // Turn off alarm after 5 minutes
#define AWAKE_TIME                          5                 // how many seconds to stay 'awake' before going back to zzz
#define MAX_VOLUME                          30.0              // Maximum volume speaker can handle

// Pins for the ILI9341
#define TFT_DC                              15
#define TFT_CS                              0

// pins for the touchscreen
#define STMPE_CS                            16
#define STMPE_IRQ                           4

#define SHIELD_RESET                        -1                  // VS1053 reset pin (unused!)
#define SHIELD_CS                           16                  // VS1053 chip select pin (output)
#define SHIELD_DCS                          15                  // VS1053 Data/command select pin (output)
#define SD_CS                               2                   // SD card chip select 

#define JSON_BUFFER_SIZE                    350
#define SETTINGS_FILE_NAME                  "settings.txt"

// Wunderground Settings
#define IS_METRIC                           false
#define OPEN_WEATHER_MAP_APP_ID             "< your ap id goes here >"
#define OPEN_WEATHER_MAP_LOCATION           "< zip code goes here >"

// Pick a language code from this list:
// Arabic - ar, Bulgarian - bg, Catalan - ca, Czech - cz, German - de, Greek - el,
// English - en, Persian (Farsi) - fa, Finnish - fi, French - fr, Galician - gl,
// Croatian - hr, Hungarian - hu, Italian - it, Japanese - ja, Korean - kr,
// Latvian - la, Lithuanian - lt, Macedonian - mk, Dutch - nl, Polish - pl,
// Portuguese - pt, Romanian - ro, Russian - ru, Swedish - se, Slovak - sk,
// Slovenian - sl, Spanish - es, Turkish - tr, Ukrainian - ua, Vietnamese - vi,
// Chinese Simplified - zh_cn, Chinese Traditional - zh_tw.

#define OPEN_WEATHER_MAP_LANGUAGE           "en"
#define MAX_FORECASTS                       5

// Google API key and URLs for automatic location and time zone lookup
#define google_apikey                       "< google api key goes here >"
#define google_apis_url                     "www.googleapis.com"
#define google_maps_apis_url                "maps.googleapis.com"
#define MAX_NUM_SSID                        5                          // Use a max of 5 SSID addresses for location lookup to keep memory usage low

class Settings  {
  public:
    DateTime AlarmTime;
    uint8_t AlarmVolume = 30;           // 0-100
    uint8_t AlarmSound = 2;             // File #:   0001.mp3, 0002.mp3, etc.
    bool AlarmEnabled = false;
    bool AlarmActive = false;
    bool AlarmDays[7];                  // Days alarm is active, 0=Sunday, 1=Monday, etc.
    bool TimeZoneFound = false;
    int DstOffset = -1;
    int RawTimeZoneOffset;
    double Latitude;
    double Longitude;

  public:
    void load();
    void save();
    uint8_t getMp3Volume();
    bool isAlarmEnabled(DateTime dateTime);
};

#endif
