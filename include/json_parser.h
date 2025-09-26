#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Arduino.h>
#include <vector>

struct WeatherInfo {
    float temp;
    int humidity;
    String description;
    unsigned long sunrise;
    unsigned long sunset;
};

struct ForecastDay {
    String dayName;
    float tempDay;
    float tempMin;
    float tempMax;
    int humidity;
    String description;
};

WeatherInfo parseWeatherData(const String& json);

std::vector<ForecastDay> parseForecast(const String& json);

String formatUnixTime(unsigned long timestamp);

#endif
