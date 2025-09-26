#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <math.h>
#include <ctype.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); 
  tft.init(240, 320);         

  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
}

static String toLowerStr(const String &s) {
  String out = s;
  for (size_t i = 0; i < out.length(); ++i) out[i] = tolower(out[i]);
  return out;
}


void drawSunIcon(int x, int y, uint16_t color) {
  int cx = x + 16, cy = y + 12;

  tft.fillCircle(cx, cy, 7, color);           
  tft.fillCircle(cx, cy, 4, ST77XX_YELLOW);   

  // promienie
  for (int i = 0; i < 8; ++i) {
    float a = i * (M_PI / 4.0f);
    int x1 = cx + int(cos(a) * 10.0f);
    int y1 = cy + int(sin(a) * 10.0f);
    int x2 = cx + int(cos(a) * 15.0f);
    int y2 = cy + int(sin(a) * 15.0f);
    tft.drawLine(x1, y1, x2, y2, color);
  }
}

void drawCloudIcon(int x, int y, uint16_t color) {

  tft.fillCircle(x + 10, y + 12, 8, color);
  tft.fillCircle(x + 18, y + 10, 7, color);
  tft.fillCircle(x + 24, y + 14, 6, color);
  tft.fillRect(x + 6, y + 14, 22, 10, color);

  tft.drawCircle(x + 10, y + 12, 8, ST77XX_WHITE);
  tft.drawCircle(x + 18, y + 10, 7, ST77XX_WHITE);
  tft.drawCircle(x + 24, y + 14, 6, ST77XX_WHITE);
  tft.drawRect(x + 6, y + 14, 22, 10, ST77XX_WHITE);
}

void drawRainIcon(int x, int y, uint16_t color) {
  drawCloudIcon(x, y, ST77XX_WHITE);

  int xs[] = {10, 16, 22};
  for (int i = 0; i < 3; ++i) {
    int cx = x + xs[i], cy = y + 24;

    tft.fillTriangle(cx, cy, cx - 2, cy + 6, cx + 2, cy + 6, color);
    tft.drawTriangle(cx, cy, cx - 2, cy + 6, cx + 2, cy + 6, color);
  }
}

void drawSnowIcon(int x, int y, uint16_t color) {

  int cx = x + 16, cy = y + 14;
  for (int i = -1; i <= 1; ++i) {
    tft.drawLine(cx - 8, cy + i, cx + 8, cy + i, color); 
    tft.drawLine(cx + i, cy - 8, cx + i, cy + 8, color); 
  }
  // przekątne
  tft.drawLine(cx - 6, cy - 6, cx + 6, cy + 6, color);
  tft.drawLine(cx - 6, cy + 6, cx + 6, cy - 6, color);
  tft.drawLine(cx - 8, cy - 4, cx + 8, cy + 4, color);
  tft.drawLine(cx - 8, cy + 4, cx + 8, cy - 4, color);
}



void showCurrentWeather(const WeatherInfo& info) {
  tft.fillScreen(ST77XX_BLACK);

  // nagłówek
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 6);
  tft.println("Aktualna pogoda");


  tft.setTextSize(4);
  tft.setCursor(10, 36);
  tft.print(int(info.temp), 1);
  tft.print(" C");

  // wilgotność i opis
  tft.setTextSize(2);
  tft.setCursor(10, 90);
  tft.print("Wilg: ");
  tft.print(info.humidity);
  tft.println("%");

  tft.setCursor(10, 116);
  tft.print("Opis: ");
  tft.println(info.description);

  // wschód/zachód
  tft.setTextSize(2);
  tft.setCursor(10, 150);
  tft.print("Wschod: ");
  tft.println(formatUnixTime(info.sunrise));
  tft.setCursor(10, 176);
  tft.print("Zachod: ");
  tft.println(formatUnixTime(info.sunset));

  // ikona po prawej
  String d = toLowerStr(info.description);
  if (d.indexOf("deszcz") >= 0 || d.indexOf("rain") >= 0) {
    drawRainIcon(180, 36, ST77XX_CYAN);
  } else if (d.indexOf("snieg") >= 0 || d.indexOf("snow") >= 0) {
    drawSnowIcon(180, 36, ST77XX_CYAN);
  } else if (d.indexOf("chmur") >= 0 || d.indexOf("cloud") >= 0) {
    drawCloudIcon(172, 36, ST77XX_WHITE);
  } else {
    drawSunIcon(172, 28, ST77XX_YELLOW);
  }
}

// Pokazuje jeden dzień prognozy (pełny ekran) 
void showDayForecast(const ForecastDay& day, size_t dayIndex) {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setCursor(10, 8);
  tft.println(day.dayName);

  tft.setTextSize(4);
  tft.setCursor(10, 44);
  tft.print(day.tempDay, 1);
  tft.print(" C");

  tft.setTextSize(2);
  tft.setCursor(10, 112);
  tft.print("Min: ");
  tft.print(int(day.tempMin), 1);
  tft.print("  Max: ");
  tft.print(int(day.tempMax), 1);

  tft.setCursor(10, 140);
  tft.print("Wilg: ");
  tft.print(day.humidity);
  tft.println("%");

  tft.setCursor(10, 168);
  tft.println(day.description);

  String d = toLowerStr(day.description);
  if (d.indexOf("deszcz") >= 0 || d.indexOf("rain") >= 0) {
    drawRainIcon(170, 40, ST77XX_CYAN);
  } else if (d.indexOf("snieg") >= 0 || d.indexOf("snow") >= 0) {
    drawSnowIcon(170, 40, ST77XX_CYAN);
  } else if (d.indexOf("chmur") >= 0 || d.indexOf("cloud") >= 0) {
    drawCloudIcon(170, 40, ST77XX_WHITE);
  } else {
    drawSunIcon(170, 32, ST77XX_YELLOW);
  }
}
