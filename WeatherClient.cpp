/**The MIT License (MIT)

  Copyright (c) 2015 by Daniel Eichhorn

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

  See more at http://blog.squix.ch
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "WeatherClient.h"
#include "Util.h"
#include "TimeZone.h"

WeatherClient::WeatherClient(boolean _isMetric, TimeZone* timeZone) {
  isMetric = _isMetric;
  _timeZone = timeZone;
}

// Added by fowlerk, 12/22/16, as an option to change metric setting other than at instantiation
void WeatherClient::initMetric(boolean _isMetric) {
  isMetric = _isMetric;
}

void WeatherClient::doUpdate() {

  OpenWeatherMapCurrent *currentWeatherClient = new OpenWeatherMapCurrent();
  currentWeatherClient->setMetric(IS_METRIC);
  currentWeatherClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  currentWeatherClient->updateCurrent(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);
  delete currentWeatherClient;
  currentWeatherClient = nullptr;

  OpenWeatherMapForecast *forecastClient = new OpenWeatherMapForecast();
  forecastClient->setMetric(IS_METRIC);
  forecastClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  uint8_t allowedHours[] = {12, 0};
  forecastClient->setAllowedHours(allowedHours, sizeof(allowedHours));
  uint8_t count = forecastClient->updateForecasts(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
  delete forecastClient;
  forecastClient = nullptr;
  /*

    time_t nowtime;
    for (uint8_t i = 0; i < count; i++) {
      Serial.printf("---\nForecast number: %d\n", i);
      // {"dt":1527066000, uint32_t observationTime;
      nowtime = forecasts[i].observationTime;
      Serial.printf("observationTime: %d, full date: %s", forecasts[i].observationTime, ctime(&nowtime));
      // "main":{
      //   "temp":17.35, float temp;
      Serial.printf("temp: %f\n", forecasts[i].temp);
      //   "temp_min":16.89, float tempMin;
      Serial.printf("tempMin: %f\n", forecasts[i].tempMin);
      //   "temp_max":17.35, float tempMax;
      Serial.printf("tempMax: %f\n", forecasts[i].tempMax);
      //   "pressure":970.8, float pressure;
      Serial.printf("pressure: %f\n", forecasts[i].pressure);
      //   "sea_level":1030.62, float pressureSeaLevel;
      Serial.printf("pressureSeaLevel: %f\n", forecasts[i].pressureSeaLevel);
      //   "grnd_level":970.8, float pressureGroundLevel;
      Serial.printf("pressureGroundLevel: %f\n", forecasts[i].pressureGroundLevel);
      //   "humidity":97, uint8_t humidity;
      Serial.printf("humidity: %d\n", forecasts[i].humidity);
      //   "temp_kf":0.46
      // },"weather":[{
      //   "id":802, uint16_t weatherId;
      Serial.printf("weatherId: %d\n", forecasts[i].weatherId);
      //   "main":"Clouds", String main;
      Serial.printf("main: %s\n", forecasts[i].main.c_str());
      //   "description":"scattered clouds", String description;
      Serial.printf("description: %s\n", forecasts[i].description.c_str());
      //   "icon":"03d" String icon; String iconMeteoCon;
      Serial.printf("icon: %s\n", forecasts[i].icon.c_str());
      Serial.printf("iconMeteoCon: %s\n", forecasts[i].iconMeteoCon.c_str());
      // }],"clouds":{"all":44}, uint8_t clouds;
      Serial.printf("clouds: %d\n", forecasts[i].clouds);
      // "wind":{
      //   "speed":1.77, float windSpeed;
      Serial.printf("windSpeed: %f\n", forecasts[i].windSpeed);
      //   "deg":207.501 float windDeg;
      Serial.printf("windDeg: %f\n", forecasts[i].windDeg);
      // rain: {3h: 0.055}, float rain;
      Serial.printf("rain: %f\n", forecasts[i].rain);
      // },"sys":{"pod":"d"}
      // dt_txt: "2018-05-23 09:00:00"   String observationTimeText;
      Serial.printf("observationTimeText: %s\n", forecasts[i].observationTimeText.c_str());
    }

  */
  time_t tnow = time(nullptr);

  Astronomy *astronomy = new Astronomy();
  moonData = astronomy->calculateMoonData(tnow);
  float lunarMonth = 29.53;
  moonAge = moonData.phase <= 4 ? lunarMonth * moonData.illumination / 2 : lunarMonth - moonData.illumination * lunarMonth / 2;
  moonAgeImage = String((char) (65 + ((uint8_t) ((26 * moonAge / 30) % 26))));
  delete astronomy;
  astronomy = nullptr;

  SunMoonCalc smCalc = SunMoonCalc(tnow, _timeZone->getLat(), _timeZone->getLong());
  sunAndMoonData = smCalc.calculateSunAndMoonData();
}

int WeatherClient::getMoonAge() {
  return moonAge;
}

String WeatherClient::getSunriseTime() {
  time_t time = currentWeather.sunrise;
  return Util::getTime(&time);
}

String WeatherClient::getSunsetTime() {
  time_t time = currentWeather.sunset;
  return Util::getTime(&time);
}

String WeatherClient::getMoonriseTime() {
  return  Util::getTime(&sunAndMoonData.moon.rise);
}

String WeatherClient::getMoonsetTime() {
  return Util::getTime(&sunAndMoonData.moon.set);
}

String WeatherClient::getCurrentTemp() {
  return String((int)(currentWeather.temp + 0.5));
}

String WeatherClient::getWeatherText() {
  return currentWeather.description;
}

String WeatherClient::getTodayIcon() {
  return getMeteoconIcon(currentWeather.icon);
}

String WeatherClient::getForecastIcon(int period) {
  return getMeteoconIcon(forecasts[period].icon);
}

String WeatherClient::getForecastTitle(int period) {
  time_t time = forecasts[period].observationTime;
  DateTime forcastTime = DateTime(time);
  int dow = forcastTime.dayOfTheWeek();
  return TimeZone::getDayString(dow);
}

String WeatherClient::getForecastLowTemp(int period) {
  return String((int)(forecasts[period].tempMin + 0.5));
}

String WeatherClient::getForecastHighTemp(int period) {
  return String((int)(forecasts[period].tempMax + 0.5));
}

String WeatherClient::getMeteoconIcon(String iconText) {

  if (iconText == "01d") return F("sunny");
  if (iconText == "02d") return F("prtsun");
  if (iconText == "03d") return F("prtcld");
  if (iconText == "04d") return F("mstcld");
  if (iconText == "09d") return F("rain");
  if (iconText == "10d") return F("rain");
  if (iconText == "11d") return F("tstorms");
  if (iconText == "13d") return F("snow");
  if (iconText == "50d") return F("fog");

  if (iconText == "01n") return F("sunny");
  if (iconText == "02n") return F("prtsun");
  if (iconText == "03n") return F("prtcld");
  if (iconText == "04n") return F("mstcld");
  if (iconText == "09n") return F("rain");
  if (iconText == "10n") return F("rain");
  if (iconText == "11n") return F("tstorms");
  if (iconText == "13n") return F("snow");
  if (iconText == "50n") return F("fog");

  return  F("unknown");

  /*
    if (iconText == F("chanceflurries")) return F("cflur");
    if (iconText == F("chancerain")) return F("crain");
    if (iconText == F("chancesleet")) return F("ccsleet");
    if (iconText == F("chancesnow")) return F("csnow");
    if (iconText == F("chancetstorms")) return F("ctstorms");
    if (iconText == F("clear")) return F("clear");
    if (iconText == F("cloudy")) return F("cloudy");
    if (iconText == F("flurries")) return F("flurries");
    if (iconText == F("fog")) return F("fog");
    if (iconText == F("hazy")) return F("hazy");
    if (iconText == F("mostlycloudy")) return F("mstcld");
    if (iconText == F("mostlysunny")) return F("mstsun");
    if (iconText == F("partlycloudy")) return F("prtcld");
    if (iconText == F("partlysunny")) return F("prtsun");
    if (iconText == F("sleet")) return F("sleet");
    if (iconText == F("rain")) return F("rain");
    if (iconText == F("snow")) return F("snow");
    if (iconText == F("sunny")) return F("sunny");
    if (iconText == F("tstorms")) return F("tstorms");

    if (iconText == F("nt_chanceflurries")) return F("cflur");
    if (iconText == F("nt_chancerain")) return F("7");
    if (iconText == F("nt_chancesleet")) return F("#");
    if (iconText == F("nt_chancesnow")) return F("#");
    if (iconText == F("nt_chancetstorms")) return F("&");
    if (iconText == F("nt_clear")) return F("2");
    if (iconText == F("nt_cloudy")) return F("Y");
    if (iconText == F("nt_flurries")) return F("9");
    if (iconText == F("nt_fog")) return F("M");
    if (iconText == F("nt_hazy")) return F("E");
    if (iconText == F("nt_mostlycloudy")) return F("5");
    if (iconText == F("nt_mostlysunny")) return F("3");
    if (iconText == F("nt_partlycloudy")) return F("4");
    if (iconText == F("nt_partlysunny")) return F("4");
    if (iconText == F("nt_sleet")) return F("9");
    if (iconText == F("nt_rain")) return F("7");
    if (iconText == F("nt_snow")) return F("#");
    if (iconText == F("nt_sunny")) return F("4");
    if (iconText == F("nt_tstorms")) return F("&");

    return F("unknown");
  */


}
