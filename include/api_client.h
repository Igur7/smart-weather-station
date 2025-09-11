#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>   

String getWeatherData(const char* apiKey, const char* city, const char* country);
String getWeatherForecast(const char* apiKey, float lat, float lon);

#endif
