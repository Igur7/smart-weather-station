#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "json_parser.h"

#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
#define TFT_BL    15

extern Adafruit_ST7789 tft;

void initDisplay();
void showCurrentWeather(const WeatherInfo& info);
void showForecast(const String& json);

#endif
