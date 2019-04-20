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

WeatherClient::WeatherClient(boolean _isMetric) {
  isMetric = _isMetric;
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
  forecastClient->updateForecasts(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
  delete forecastClient;
  forecastClient = nullptr;

  Astronomy *astronomy = new Astronomy();
  moonData = astronomy->calculateMoonData(time(nullptr));
  float lunarMonth = 29.53;
  moonAge = moonData.phase <= 4 ? lunarMonth * moonData.illumination / 2 : lunarMonth - moonData.illumination * lunarMonth / 2;
  moonAgeImage = String((char) (65 + ((uint8_t) ((26 * moonAge / 30) % 26))));
  delete astronomy;
  astronomy = nullptr;
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
  return "";
}

String WeatherClient::getMoonsetTime() {
  return "";
}

String WeatherClient::getCurrentTemp() {
  // int currentTemp = round(currentWeather.temp*10.0)/10.0;

  int currentTemp = (int)(currentWeather.temp + 0.5);
  return String(currentTemp);
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
