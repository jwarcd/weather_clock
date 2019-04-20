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

#include "settings.h"
#include "Util.h"

#include <SD.h>
#include <ArduinoJson.h>

bool Settings::isAlarmEnabled(DateTime dateTime) {
  uint8_t dayOfTheWeek = dateTime.dayOfTheWeek();
  //Sprintln("Alarm enabled:  " + String(AlarmEnabled) + ", dayOfTheWeek: " + String(dayOfTheWeek) + ", DaysOfWeek: " + String(AlarmDays[dayOfTheWeek]));
  return AlarmEnabled && AlarmDays[dayOfTheWeek];
}

uint8_t Settings::getMp3Volume() {
  return (uint8_t)((AlarmVolume / 100.0) * MAX_VOLUME);
}

void Settings::load() {
  Sprint(F("Loading Settings: "));

  File file;
  if ((file = SD.open(SETTINGS_FILE_NAME, FILE_READ)) == NULL) {
    Sprintln(F("settings.txt not found"));
    return;
  }

  char json[JSON_BUFFER_SIZE];
  int length = 0;

  // read data into String
  while (file.available() && length < JSON_BUFFER_SIZE - 1)
  {
    json[length++] = (char)file.read();
  }

  json[length] = '\0';
  file.close();

  Sprintln(String(json));

  DynamicJsonDocument jsonBuffer2(1024);
  deserializeJson(jsonBuffer2, json);
  JsonObject root = jsonBuffer2.as<JsonObject>();

  uint32_t alarm = root[F("ATime")];
  AlarmTime = DateTime(alarm);

  AlarmEnabled = root[F("AEnable")];
  AlarmVolume = root[F("AVol")];
  AlarmSound = root[F("ASound")];
  for (int i = 0; i < 7; i++) {
    AlarmDays[i] = root["D" + String(i)];
  }

  DstOffset = root[F("DstOff")];
  RawTimeZoneOffset = root[F("RawTZOff")];

  Latitude = root[F("Lat")];
  Longitude = root[F("Long")];
}

void Settings::save() {
  Sprintln(F("saveSettings: "));

  //  We don't want to append to the existing file so remove the existing settings file
  SD.remove(SETTINGS_FILE_NAME);

  File file;
  if ((file = SD.open(SETTINGS_FILE_NAME, FILE_WRITE)) == NULL) {
    Sprintln(F("saveSettings: settings.txt not found"));
    return;
  }

  StaticJsonDocument<JSON_BUFFER_SIZE> root;

  long alarm = AlarmTime.unixtime();
  root[F("ATime")] = alarm;
  root[F("AEnable")] = AlarmEnabled;
  root[F("AVol")] = AlarmVolume;
  root[F("ASound")] = AlarmSound;
  for (int i = 0; i < 7; i++) {
    root["D" + String(i)] = AlarmDays[i];
  }

  root[F("DstOff")] = DstOffset;
  root[F("RawTZOff")] = RawTimeZoneOffset;

  root[F("Lat")] = Latitude;
  root[F("Long")] = Longitude;

  serializeJsonPretty(root, file);
  file.close();
}
