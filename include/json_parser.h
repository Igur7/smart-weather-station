#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Arduino.h>

struct WeatherInfo {
    float temp;
    int humidity;
    String description;
    unsigned long sunrise;
    unsigned long sunset;
};

struct ForecastDay {
    float tempDay;
    float tempMin;
    float tempMax;
    int humidity;
    String description;
};

WeatherInfo parseWeatherData(const String& json);

size_t parseForecast(const String& json, ForecastDay forecast[], size_t maxDays);

String formatUnixTime(unsigned long timestamp);

#endif
