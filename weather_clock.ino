w/*
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

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_MQTT_FONA.h>

#define FS_NO_GLOBALS

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>       // Core graphics library
#include <Adafruit_ILI9341.h>   // display library
#include <ILI9341_SPI.h>
//#include <ArduinoOTA.h>
#include <WiFiManager.h>

/*************************************************************
   NOTE: Update settings.h to configure your settings!!!
 ************************************************************/
#include "settings.h"

#include "ArialRoundedMTBold_14.h"
#include "Util.h"
#include "TouchScreen.h"
#include <SPI.h>
#include "WeatherClient.h"
#include <DFRobotDFPlayerMini.h>
#include <SD.h>

#include "GfxUi.h"
#include "Button.h"
#include "WeatherPage.h"
#include "AlarmPage.h"
#include "SetAlarmPage.h"
#include "TimeZone.h"

// MP3 Player
DFRobotDFPlayerMini mp3;

// TFT Display and touch screen
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
GfxUi ui = GfxUi(&tft);
TouchScreen touch = TouchScreen(&tft);

// Weather
WeatherClient weatherClient(IS_METRIC);

// Prototypes
void configModeCallback (WiFiManager *myWiFiManager);

// Saved settings
Settings settings;

// Time and time zone handling
TimeZone timeZone(&settings);

// WiFiManager
// Local initialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Pages
Page* currentPage;
WeatherPage weatherPage = WeatherPage(WEATHER_PAGE, &weatherClient, &timeZone, &settings);
AlarmPage alarmPage = AlarmPage(ALARM_ACTIVE_PAGE, &mp3);
SetAlarmPage setAlarmPage = SetAlarmPage(SET_ALARM_PAGE, &timeZone, &mp3);

void setup() {
  Serial.begin(9600);

  Serial.setDebugOutput(true);

  // Make sure I2C bus is in the right state
  Util::clearBusI2C();

  setupMp3();

  if (!touch.begin()) {
    Sprintln(F("TouchScreen not found?"));
  }

  // Initialized TFT display
  tft.begin();
  ui.clearScreen();
  ui.setFont(&ArialRoundedMTBold_14);
  ui.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  ui.setTextAlignment(CENTER);
  ui.drawString(120, 160, F("Connecting to WiFi"));

  // Wifi initialization
  //wifiManager.resetSettings();  // Uncomment for testing wifi manager
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect();
  String hostname(OTA_HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);

  timeZone.begin();

  // OTA Setup
  //ArduinoOTA.setHostname((const char *)hostname.c_str());
  //ArduinoOTA.setPassword(OTA_PASSWORD);
  //ArduinoOTA.begin();

  SD.begin(SD_CS);
  delay(100);

  ui.drawString(120, 160, F("Loading settings..."));
  settings.load();

  setPage(&weatherPage);
}

void loop() {
  // Handle OTA update requests
  // ArduinoOTA.handle();

  timeZone.updateTimeData();

  checkAlarm();

  int nextPage = currentPage->loop(&ui, touch.isTouched(), touch.getTouchPoint());

  if (currentPage->getId() == nextPage)
    return;

  settings.AlarmActive = false;
  mp3.stop();

  Settings setAlarmSettings;

  switch (currentPage->getId()) {
    case WEATHER_PAGE :
      break;
    case ALARM_ACTIVE_PAGE :
      break;
    case SET_ALARM_PAGE :
      {
        settings = setAlarmPage.getSettings();
        settings.save();
      }
      break;
    default:
      Sprintln(F("Unknown Page"));
      break;
  }

  switch (nextPage) {
    case WEATHER_PAGE :
      setPage(&weatherPage);
      break;
    case ALARM_ACTIVE_PAGE :
      setPage(&alarmPage);
      break;
    case SET_ALARM_PAGE :
      setAlarmPage.setSettings(settings);
      setPage(&setAlarmPage);
      break;
    default:
      Sprintln(F("Unknown Page"));
      break;
  }
}

void checkAlarm() {
  DateTime nowTime = timeZone.now();

  if (settings.isAlarmEnabled(nowTime) && !settings.AlarmActive &&
      (settings.AlarmTime.hour()   == nowTime.hour()) &&
      (settings.AlarmTime.minute() == nowTime.minute()) &&
      (settings.AlarmTime.second() == nowTime.second())) {
    Sprint(F("Alarm Active: time = "));
    Sprintln(Util::formatTime(nowTime.hour(), nowTime.minute(), nowTime.second()));

    settings.AlarmActive = true;
    mp3.volume(1);  // AlarmPage will gradually increase volume
    mp3.loop(settings.AlarmSound);
    alarmPage.setSettings(settings);
    setPage(&alarmPage);
  }
}

void setPage(Page* page) {
  currentPage = page;
  ui.clearScreen();
  currentPage->setup(&ui);

  Sprint(F("setting page to: "));
  Sprintln(String(page->getId()));
}

void setupMp3() {
  mp3.begin(Serial);
  delay(900);  // allow time for BY8001 cold boot
  mp3.volume(0);
}

// Called if WiFi has not been configured yet
void configModeCallback (WiFiManager *myWiFiManager) {
  ui.setTextAlignment(CENTER);
  ui.setFont(&ArialRoundedMTBold_14);
  ui.setTextColor(ILI9341_CYAN);
  ui.drawString(120, 28, F("Wifi Manager"));
  ui.drawString(120, 42, F("Please connect to AP"));
  ui.setTextColor(ILI9341_WHITE);
  ui.drawString(120, 56, myWiFiManager->getConfigPortalSSID());
  ui.setTextColor(ILI9341_CYAN);
  ui.drawString(120, 70, F("To setup Wifi Configuration"));
}
