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
#include "WundergroundClient.h"
#include "Util.h"

bool usePM = false; // Set to true if you want to use AM/PM time disaply
bool isPM = false; // JJG added ///////////

/*
NOTE: Jared Gaillard - This is a modified version of the WundergroundClient by Daniel Eichhorn.  
      To reduce the memory footprint unnecessary code was removed, and the use of Strings was reduced.
*/

WundergroundClient::WundergroundClient(boolean _isMetric) {
  isMetric = _isMetric;
}

// Added by fowlerk, 12/22/16, as an option to change metric setting other than at instantiation
void WundergroundClient::initMetric(boolean _isMetric) {
	isMetric = _isMetric;
}
// end add fowlerk, 12/22/16

void WundergroundClient::updateConditions(String apiKey, String language, String country, String city) {
  isForecast = false;
  doUpdate("/api/" + apiKey + "/conditions/lang:" + language + "/q/" + country + "/" + city + ".json");
}

// wunderground change the API URL scheme:
// http://api.wunderground.com/api/<API-KEY>/conditions/lang:de/q/zmw:00000.215.10348.json
void WundergroundClient::updateConditions(String apiKey, String language, String zmwCode) {
  isForecast = false;
  doUpdate("/api/" + apiKey + "/conditions/lang:" + language + "/q/zmw:" + zmwCode + ".json");
}

void WundergroundClient::updateForecast(String apiKey, String language, String country, String city) {
  isForecast = true;
  doUpdate("/api/" + apiKey + "/forecast10day/lang:" + language + "/q/" + country + "/" + city + ".json");
}

// JJG added ////////////////////////////////
void WundergroundClient::updateAstronomy(String apiKey, String language, String country, String city) {
  isForecast = true;
  doUpdate("/api/" + apiKey + "/astronomy/lang:" + language + "/q/" + country + "/" + city + ".json");
}
// end JJG add  ////////////////////////////////////////////////////////////////////

void WundergroundClient::doUpdate(String url) {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("api.wunderground.com", httpPort)) {
    Sprintln(F("connection failed"));
    return;
  }

  Sprint(F("Requesting URL: "));
  Sprintln(url);

  // This will send the request to the server
  client.print(String(F("GET ")) + url);
  client.print(F(" HTTP/1.1\r\nHost: api.wunderground.com\r\nConnection: close\r\n\r\n"));
  int retryCounter = 0;
  while(!client.available()) {
    delay(1000);
    retryCounter++;
    if (retryCounter > 10) {
      return;
    }
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(false);
  while(client.connected()) {
    while((size = client.available()) > 0) {
      c = client.read();
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }
}

void WundergroundClient::whitespace(char c) {
  //Sprintln(F("whitespace"));
}

void WundergroundClient::startDocument() {
  //Sprintln(F("start document"));
}

void WundergroundClient::key(String key) {
  currentKey = String(key);
//	Restructured following logic to accomodate the multiple types of JSON returns based on the API.  This was necessary since several
//	keys are reused between various types of API calls, resulting in confusing returns in the original function.  Various booleans
//	now indicate whether the JSON stream being processed is part of the text forecast (txt_forecast), the first section of the 10-day
//	forecast API that contains detailed text for the forecast period; the simple forecast (simpleforecast), the second section of the
//	10-day forecast API that contains such data as forecast highs/lows, conditions, precipitation / probabilities; the current
//	observations (current_observation), from the observations API call; or alerts (alerts), for the future) weather alerts API call.
//		Added by fowlerk...18-Dec-2016
  if (currentKey == F("txt_forecast")) {
	isForecast = true;
	isCurrentObservation = false;	// fowlerk
    isSimpleForecast = false;		// fowlerk
  }
  if (currentKey == F("simpleforecast")) {
    isSimpleForecast = true;
	isCurrentObservation = false;	// fowlerk
	isForecast = false;				// fowlerk
  }
//  Added by fowlerk...
  if (currentKey == F("current_observation")) {
    isCurrentObservation = true;
	isSimpleForecast = false;
	isForecast = false;
  }
// end fowlerk add 
}

void WundergroundClient::value(String value) {
  if (currentKey == F("ageOfMoon")) {
    moonAge = value.toInt();
  }

  if (currentParent == F("sunrise")) {      // Has a Parent key and 2 sub-keys
	if (currentKey == F("hour")) {
		int tempHour = value.toInt();    // do this to concert to 12 hour time (make it a function!)
		if (usePM && tempHour > 12){
			tempHour -= 12;
			isPM = true;
		}
		else isPM = false;
		char tempHourBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempHourBuff, PSTR("%2d"), tempHour);			// fowlerk add for formatting, 12/22/16
		sunriseTime = String(tempHourBuff);				// fowlerk add for formatting, 12/22/16
        //sunriseTime = value;
      }
	if (currentKey == F("minute")) {
		char tempMinBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempMinBuff, PSTR("%02d"), value.toInt());	// fowlerk add for formatting, 12/22/16
		sunriseTime += ":" + String(tempMinBuff);		// fowlerk add for formatting, 12/22/16
		if (isPM) sunriseTime += F("pm");
		else if (usePM) sunriseTime += F("am");
	}
  }

  if (currentParent == F("sunset")) {      // Has a Parent key and 2 sub-keys
	if (currentKey == F("hour")) {
		int tempHour = value.toInt();   // do this to concert to 12 hour time (make it a function!)
		if (usePM && tempHour > 12){
			tempHour -= 12;
			isPM = true;
		}
		else isPM = false;
		char tempHourBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempHourBuff, PSTR("%2d"), tempHour);			// fowlerk add for formatting, 12/22/16
		sunsetTime = String(tempHourBuff);				// fowlerk add for formatting, 12/22/16
       // sunsetTime = value;
      }
	if (currentKey == F("minute")) {
		char tempMinBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempMinBuff, PSTR("%02d"), value.toInt());	// fowlerk add for formatting, 12/22/16
		sunsetTime += ":" + String(tempMinBuff);		// fowlerk add for formatting, 12/22/16
		if (isPM) sunsetTime += F("pm");
		else if(usePM) sunsetTime += F("am");
    }
  }

  if (currentParent == F("moonrise")) {      // Has a Parent key and 2 sub-keys
	if (currentKey == F("hour")) {
		int tempHour = value.toInt();   // do this to concert to 12 hour time (make it a function!)
		if (usePM && tempHour > 12){
			tempHour -= 12;
			isPM = true;
		}
		else isPM = false;
		char tempHourBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempHourBuff, PSTR("%2d"), tempHour);			// fowlerk add for formatting, 12/22/16
		moonriseTime = String(tempHourBuff);			// fowlerk add for formatting, 12/22/16	
       // moonriseTime = value;
      }
	if (currentKey == F("minute")) {
		char tempMinBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempMinBuff, PSTR("%02d"), value.toInt());	// fowlerk add for formatting, 12/22/16
		moonriseTime += ":" + String(tempMinBuff);		// fowlerk add for formatting, 12/22/16
		if (isPM) moonriseTime += F("pm");
		else if (usePM) moonriseTime += F("am");
    }
  }

  if (currentParent == F("moonset")) {      // Not used - has a Parent key and 2 sub-keys
	if (currentKey == F("hour")) {
		char tempHourBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempHourBuff, PSTR("%2d"), value.toInt());	// fowlerk add for formatting, 12/22/16
		moonsetTime = String(tempHourBuff);				// fowlerk add for formatting, 12/22/16	
    }
	if (currentKey == F("minute")) {
		char tempMinBuff[3] = "";						// fowlerk add for formatting, 12/22/16
		sprintf_P(tempMinBuff, PSTR("%02d"), value.toInt());	// fowlerk add for formatting, 12/22/16
		moonsetTime += ":" + String(tempMinBuff);		// fowlerk add for formatting, 12/22/16
    }
  }

// end JJG add  ////////////////////////////////////////////////////////////////////

  if (currentKey == F("temp_f") && !isMetric) {
    currentTemp = value;
  }
  if (currentKey == F("temp_c") && isMetric) {
    currentTemp = value;
  }
  if (currentKey == F("icon")) {
    if (isForecast && !isSimpleForecast && currentForecastPeriod < MAX_FORECAST_PERIODS) {
      Sprintln(String(currentForecastPeriod) + ": " + value + ":" + currentParent);
      forecastIcon[currentForecastPeriod] = value;
    }
    // if (!isForecast) {													// Removed by fowlerk
    if (isCurrentObservation && !(isForecast || isSimpleForecast)) {		// Added by fowlerk
      weatherIcon = value;
    }
  }
  if (currentKey == F("weather")) {
    weatherText = value;
  }
  if (currentKey == F("period")) {
    currentForecastPeriod = value.toInt();
  }
  
  // Modified below line to add check to ensure we are processing the 10-day forecast
  // before setting the forecastTitle (day of week of the current forecast day).
  // (The keyword title is used in both the current observation and the 10-day forecast.)
  //		Modified by fowlerk  
  // if (currentKey == "title" && currentForecastPeriod < MAX_FORECAST_PERIODS) {				// Removed, fowlerk
  if (currentKey == F("title") && isForecast && currentForecastPeriod < MAX_FORECAST_PERIODS) {
      Sprintln(String(currentForecastPeriod) + ": " + value);
      forecastTitle[currentForecastPeriod] = value;
  }

  // The detailed forecast period has only one forecast per day with low/high for both
  // night and day, starting at index 1.
  int dailyForecastPeriod = (currentForecastPeriod - 1) * 2;

  if (currentKey == F("fahrenheit") && !isMetric && dailyForecastPeriod < MAX_FORECAST_PERIODS) {

      if (currentParent == F("high")) {
        forecastHighTemp[dailyForecastPeriod] = value;
      }
      if (currentParent == F("low")) {
        forecastLowTemp[dailyForecastPeriod] = value;
      }
  }
  if (currentKey == F("celsius") && isMetric && dailyForecastPeriod < MAX_FORECAST_PERIODS) {

      if (currentParent == F("high")) {
        Sprintln(String(currentForecastPeriod) + ": " + value);
        forecastHighTemp[dailyForecastPeriod] = value;
      }
      if (currentParent == F("low")) {
        forecastLowTemp[dailyForecastPeriod] = value;
      }
  }
}

void WundergroundClient::endArray() {

}

void WundergroundClient::startObject() {
  currentParent = currentKey;
}

void WundergroundClient::endObject() {
  currentParent = "";
}

void WundergroundClient::endDocument() {

}

void WundergroundClient::startArray() {

}

int WundergroundClient::getMoonAge() {
  return moonAge;
}

String WundergroundClient::getSunriseTime() {
  return sunriseTime;
 }

String WundergroundClient::getSunsetTime() {
  return sunsetTime;
 }

String WundergroundClient::getMoonriseTime() {
  return moonriseTime;
 }

String WundergroundClient::getMoonsetTime() {
  return moonsetTime;
 }

// end JJG add ////////////////////////////////////////////////////////////////////////////////////////////

String WundergroundClient::getCurrentTemp() {
  return currentTemp;
}

String WundergroundClient::getWeatherText() {
  return weatherText;
}

String WundergroundClient::getTodayIcon() {
  return getMeteoconIcon(weatherIcon);
}

String WundergroundClient::getForecastIcon(int period) {
  return getMeteoconIcon(forecastIcon[period]);
}

String WundergroundClient::getForecastTitle(int period) {
  return forecastTitle[period];
}

String WundergroundClient::getForecastLowTemp(int period) {
  return forecastLowTemp[period];
}

String WundergroundClient::getForecastHighTemp(int period) {
  return forecastHighTemp[period];
}

String WundergroundClient::getMeteoconIcon(String iconText) {

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

}
