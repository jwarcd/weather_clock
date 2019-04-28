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

#include "TimeZone.h"
#include "Util.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <NtpClientLib.h> // https://github.com/gmag11/NtpClient
#include <ArduinoJson.h>

extern "C" {
#include "user_interface.h"
}

TimeZone::TimeZone(Settings* settings) {
  _settings = settings;
  ssid[0] = '\0';
}

void TimeZone::begin() {
  Sprintln();

  if (!_rtc.begin()) {
    Sprintln(F("Couldn't find RTC"));
    while (1);
  }

  NTP.onNTPSyncEvent([&](NTPSyncEvent_t event) {
    Sprintln(F("NTP.onNTPSyncEvent"));
    ntpSyncEventTriggered = true;
    if (event) {
      Sprint(F("Time Sync error: "));
      if (event == noResponse) {
        Sprintln(F("NTP server not reachable"));
      }
      else if (event == invalidAddress) {
        Sprintln(F("Invalid NTP server address"));
      }
    }
    else {
      Sprint(F("Got NTP time: "));
      Sprintln(NTP.getTimeDateString(NTP.getLastNTPSync()));
    }
  });

  if (!NTP.begin(F("pool.ntp.org"), 0, false)) {
    Sprintln(F("NTP.begin failed."));
    while (1);
  }
}

DateTime TimeZone::now() {
  DateTime nowTime = _rtc.now();
  //String timeText = String(nowTime.hour()) + ":" + String(nowTime.minute()) + ":" + String(nowTime.second());
  //Sprint(F("nowTime = "));
  //Sprintln(timeText);

  int offsetSeconds = _settings->DstOffset + _settings->RawTimeZoneOffset;
  nowTime = DateTime(nowTime.unixtime() + offsetSeconds);

  //timeText = String(nowTime.hour()) + ":" + String(nowTime.minute()) + ":" + String(nowTime.second());
  //Sprint(F("nowTime2 = "));
  //Sprintln(timeText);

  return nowTime;
}

double TimeZone::getLat() {
  return latitude;
}

double TimeZone::getLong() {
  return longitude;
}

void TimeZone::adjust(DateTime value) {
  _rtc.adjust(value);
}

String TimeZone::getDateString() {
  DateTime nowTime = now();

  uint8_t dayOfWeek = nowTime.dayOfTheWeek();
  String dayText = getDayString(dayOfWeek);

  uint8_t month = nowTime.month();
  String monthText = getMonthString(month);

  String date = dayText;
  date += F(", ");
  date += String(nowTime.day());
  date += F(" ");
  date += monthText;
  //date += F(" ");
  //date += String(nowTime.year());

  return date;
}

String TimeZone::getDayString(uint8_t dayOfWeek) {
  switch (dayOfWeek) {
    case 0:
      return F("Sun");
    case 1:
      return F("Mon");
    case 2:
      return F("Tue");
    case 3:
      return F("Wed");
    case 4:
      return F("Thr");
    case 5:
      return F("Fri");
    case 6:
      return F("Sat");
    default:
      return F("Unk");
  }
}

String TimeZone::getMonthString(uint8_t month) {
  switch (month) {
    case 1:
      return F("Jan");
    case 2:
      return F("Feb");
    case 3:
      return F("Mar");
    case 4:
      return F("Apr");
    case 5:
      return F("May");
    case 6:
      return F("Jun");
    case 7:
      return F("Jul");
    case 8:
      return F("Aug");
    case 9:
      return F("Sep");
    case 10:
      return F("Oct");
    case 11:
      return F("Nov");
    case 12:
      return F("Dec");
    default:
      return F("Unk");
  }
}

void TimeZone::updateTimeData() {
  if (!ntpSyncEventTriggered) {
    return;
  }

  // Check if we should update weather information
  if (((millis() - lastTimeUpdate) > (1000 * UPDATE_TIME_INTERVAL_SECS)) || (lastTimeUpdate == 0) ) {
    Sprintln(F("Updating time..."));

    time_t epochTime = NTP.getTime();
    DateTime newTime = DateTime(epochTime);
    Sprint(F(" NTP time = "));
    Sprint(newTime.hour());
    Sprint(F(":"));
    Sprint(newTime.minute());
    Sprint(F(":"));
    Sprintln(newTime.second());
    Sprint(F(" NTP date = "));
    Sprint(newTime.month());
    Sprint(F("/"));
    Sprint(newTime.day());
    Sprint(F("/"));
    Sprintln(newTime.year());

    adjust(newTime);

    DateTime nowTime = _rtc.now();
    Sprint(F(" RTC Time = "));
    Sprint(nowTime.hour());
    Sprint(F(":"));
    Sprint(nowTime.minute());
    Sprint(F(":"));
    Sprintln(nowTime.second());
    Sprint(F(" RTC date = "));
    Sprint(nowTime.month());
    Sprint(F("/"));
    Sprint(nowTime.day());
    Sprint(F("/"));
    Sprintln(nowTime.year());

    lastTimeUpdate = millis();

    scanAndLocateTimeZone();
  }
}

String TimeZone::getWifiAccessPointJson() {
  String jsonText;
  DynamicJsonDocument jsonBuffer(2048);

  memset(ssid, 0, MAX_SSID_LEN);
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Sprintln(F("No Wifi networks found!"));
  } else {
    Sprint(F("Wifi networks found: "));
    jsonBuffer[F("considerIp")] = F("false");
    JsonArray data = jsonBuffer.createNestedArray(F("wifiAccessPoints"));

    // Only use a max of MAX_NUM_SSID addresses to keep memory usage low
    for (int i = 0; i < n && i < MAX_NUM_SSID; i++) {
      DynamicJsonDocument jsonBuffer2(256);
      JsonObject wifiAP = jsonBuffer2.to<JsonObject>();
      Sprint(WiFi.SSID(i));
      Sprint(F(", "));
      wifiAP[F("macAddress")] = WiFi.BSSIDstr(i);
      wifiAP[F("signalStrength")] = WiFi.RSSI(i);
      wifiAP[F("signalToNoiseRatio")] = 0;
      data.add(wifiAP);
    }
    Sprintln();

    serializeJsonPretty(jsonBuffer, jsonText);
  }

  yield();

  return jsonText;
}

/* Scan available networks and request Google to determine your location.

   JSON returned from Google API:
   {
    "location": {
     "lat": 42.5887338,
     "lng": -71.0938992
    },
    "accuracy": 39.0
   }

*/
bool TimeZone::getLatLong(double &latitude, double &longitude) {

  // We only need to get lat, long the first time after power up
  if (_settings->TimeZoneFound && _settings->Latitude != 0 && _settings->Longitude != 0) {
    latitude = _settings->Latitude;
    longitude = _settings->Longitude;
    return true;
  }

  latitude = 0;
  longitude = 0;

  Sprintln("Free RAM: " + String(system_get_free_heap_size()));

  String postdata = getWifiAccessPointJson();

  String url = F("/geolocation/v1/geolocate?key=");
  url += String(google_apikey);
  String resultdata;
  int errorCode;
  resultdata = httpsPost(google_apis_url, url, F("application/json"), postdata, errorCode);

  Sprintln("resultdata: " + resultdata);

  DynamicJsonDocument jsonBuffer(2048);
  DeserializationError error = deserializeJson(jsonBuffer, resultdata);

  if (error) {
    Sprintln(String(F("getLatLong: JSON parsing failed! result data: ")) + resultdata);
    return false;
  }

  latitude = jsonBuffer[F("location")][F("lat")];
  longitude = jsonBuffer[F("location")][F("lng")];
  double accuracy = jsonBuffer[F("accuracy")];

  _settings->Latitude = latitude;
  _settings->Longitude = longitude;

#ifdef DEBUG
  Sprint(F("Location Found: "));
  Serial.print(latitude, 5);
  Sprint(F(", "));
  Serial.print(longitude, 5);
  Sprint(F(", accuracy: "));
  Serial.println(accuracy, 1);
#endif

  yield();

  return true;
}

/* Find the time zone.

     JSON returned from Google API:

     {
        "dstOffset" : 3600,
        "rawOffset" : -18000,
        "status" : "OK",
        "timeZoneId" : "America/New_York",
        "timeZoneName" : "Eastern Daylight Time"
     }

*/
bool TimeZone::getTimeZone() {
  int errorCode;

  Sprintln();
  Sprintln(F("Connecting to Google apis to get location data..."));

  //Sprintln("Free RAM: " + String(system_get_free_heap_size()));
  if (!getLatLong(latitude, longitude)) {
    Sprintln(F("Error getting Lat, Long, using time zone from settings."));
    return false;
  }

  DateTime utcTime = _rtc.now();
  String url = F("/maps/api/timezone/json?location=");
  url = url + String(latitude) + "," + String(longitude) + String(F("&timestamp=")) + String(utcTime.unixtime()) + String(F("&key=")) + String(google_apikey);

  String resultdata = httpsGet(google_maps_apis_url, url, errorCode);

  DynamicJsonDocument jsonBuffer2(1024);
  DeserializationError error = deserializeJson(jsonBuffer2, resultdata);
  JsonObject root2 = jsonBuffer2.as<JsonObject>();

  if (error) {
    Sprint(F("JSON parsing failed! error: "));
    Sprintln(error.c_str());
    return false;
  }

  _settings->DstOffset = (int)root2[F("dstOffset")];
  _settings->RawTimeZoneOffset = (int)root2[F("rawOffset")];
  _settings->TimeZoneFound = true;

  Sprint(F("TimeZone Found, DstOffset = "));
  Sprint(_settings->DstOffset);
  Sprint(F(", RawTimeZoneOffset = "));
  Sprintln(_settings->RawTimeZoneOffset);
  Sprintln();

  yield();

  return true;
}

/*
    Scan available networks and request Google to determine your location.
*/
void TimeZone::scanAndLocateTimeZone() {
  if (getTimeZone()) {
    _settings->save();
  }
}

String TimeZone::httpsGet(const char* host, String url,  int &errorCode) {
  WiFiClientSecure client;

  Sprint(F("HOST: "));
  Sprintln(host);
  Sprint(F("URL: "));
  Sprintln(url);

  client.setInsecure();

  if (client.connect(host, 443)) {
    client.print(F("GET "));
    client.print(F("https://"));
    client.print(String(host));
    client.print(url);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(String(host));
    client.println(F("Connection: close"));
    client.println();

    String response = "";
    while (client.connected()) {
      response = response + client.readString();
    }
    response = response + client.readString();

    errorCode = response.substring(response.indexOf(' ') + 1).toInt();
    if (errorCode == 0) {
      errorCode = 444;
      return F("No Response");
    }

    int bodypos =  response.indexOf(F("\r\n\r\n")) + 4;
    return response.substring(bodypos);
  }
  else {
    errorCode = 600;
    return F("Network Unreachable");
  }
}

String TimeZone::httpsPost(const char* host, String url, String contentType, String data, int &errorCode) {
  WiFiClientSecure client;

  Sprint(F("HOST: "));
  Sprintln(host);
  Sprint(F("URL: "));
  Sprintln(url);

  client.setInsecure();

  if (client.connect(String(host), 443)) {
    client.print(F("POST "));
    client.print(F("https://"));
    client.print(String(host));
    client.print(url);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(String(host));
    client.println(F("Mozilla/5.0 (Linux; Android 6.0.1; Nexus 6P Build/MMB29P) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.83 Mobile Safari/537.36"));
    client.println(F("Connection: close"));
    client.print(F("Content-Type: "));
    client.println(contentType);
    client.print(F("Content-Length: "));
    client.println(data.length());
    client.println();
    client.print(data);

    String response;
    while (client.connected()) {
      response = response + client.readString();
    }
    response = response + client.readString();

    errorCode = response.substring(response.indexOf(' ') + 1).toInt();
    if (errorCode == 0) {
      errorCode = 444;
      return F("No Response");
    }

    int bodypos = response.indexOf(F("\r\n\r\n")) + 4;
    return response.substring(bodypos);
  }
  else {
    errorCode = 600;
    return F("Network Unreachable");
  }
}
