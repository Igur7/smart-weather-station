#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "json_parser.h"

#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define TFT_BL   15

void initDisplay();
void showCurrentWeather(const WeatherInfo& info, float sensorTemp);
void showForecastRow(const ForecastDay& day);
void drawSunIcon(int x, int y, uint16_t color);
void drawCloudIcon(int x, int y, uint16_t color);
void drawRainIcon(int x, int y, uint16_t color);
void drawSnowIcon(int x, int y, uint16_t color);
void drawDropIcon(int x, int y, uint16_t color);
void drawSunWithCloudIcon(int x, int y, uint16_t color);
void drawSunriseIcon(int x, int y, uint16_t color);
void drawSunsetIcon(int x, int y, uint16_t color);
void drawThermometerIcon(int x, int y, uint16_t color);

#endif
