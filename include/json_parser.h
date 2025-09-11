#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <Arduino.h>

struct WeatherInfo {
    float temp;
    int humidity;
    String description;
    unsigned long sunrise;
    unsigned sunset;
};

struct ForecastDay {
    float tempDay;
    float tempMin;
    float tempMax;
    int humidity;
    String description;
};

WeatherInfo parseWeatherData(const String& json);
void parseForecast(const String& json);
String formatUnixTime(unsigned long timestamp);

#endif
