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
}

static String toLowerStr(const String &s) {
    String out = s;
    for (size_t i = 0; i < out.length(); ++i) out[i] = tolower(out[i]);
    return out;
}

void drawSunIcon(int x, int y, uint16_t color) {
    int cx = x + 20, cy = y + 20;
    tft.fillCircle(cx, cy, 14, color);
    for (int i = 0; i < 8; ++i) {
        float a = i * (M_PI / 4);
        int x1 = cx + int(cos(a) * 18);
        int y1 = cy + int(sin(a) * 18);
        int x2 = cx + int(cos(a) * 26);
        int y2 = cy + int(sin(a) * 26);
        tft.drawLine(x1, y1, x2, y2, color);
    }
}

void drawCloudIcon(int x, int y, uint16_t color) {
    tft.fillCircle(x + 15, y + 18, 12, color);
    tft.fillCircle(x + 28, y + 12, 10, color);
    tft.fillCircle(x + 40, y + 18, 12, color);
    tft.fillRect(x + 15, y + 18, 25, 15, color);
}

void drawRainIcon(int x, int y, uint16_t color) {
    drawCloudIcon(x, y, ST77XX_WHITE);
    for (int i = 0; i < 3; ++i) {
        int cx = x + 20 + i * 10;
        int cy = y + 35;
        tft.fillTriangle(cx, cy, cx - 3, cy + 10, cx + 3, cy + 10, color);
    }
}

void drawSnowIcon(int x, int y, uint16_t color) {
    int cx = x + 25, cy = y + 25;
    for (int i = -1; i <= 1; ++i) {
        tft.drawLine(cx - 10, cy + i, cx + 10, cy + i, color);
        tft.drawLine(cx + i, cy - 10, cx + i, cy + 10, color);
    }
    tft.drawLine(cx - 8, cy - 8, cx + 8, cy + 8, color);
    tft.drawLine(cx - 8, cy + 8, cx + 8, cy - 8, color);
}

void drawDropIcon(int x, int y, uint16_t color) {
    tft.fillCircle(x, y + 5, 4, color);
    tft.fillTriangle(x - 4, y + 5, x + 4, y + 5, x, y - 5, color);
}

void drawSunWithCloudIcon(int x, int y, uint16_t color) {
    tft.fillCircle(x, y, 14, color);         
    tft.drawCircle(x, y, 14, ST77XX_WHITE);
    tft.fillCircle(x + 12, y + 10, 8, ST77XX_WHITE);
    tft.fillCircle(x + 20, y + 8, 7, ST77XX_WHITE);
    tft.fillRect(x + 6, y + 10, 24, 8, ST77XX_WHITE);
}

void showCurrentWeather(const WeatherInfo& info, float sensorTemp) {
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(10, 6);
    tft.println("Aktualna pogoda");

    tft.setTextSize(4);
    tft.setCursor(10, 36);
    tft.print(int(info.temp));
    tft.print(" C");

    tft.setTextSize(2);
    tft.setCursor(10, 90);
    drawDropIcon(10, 90, ST77XX_CYAN);
    tft.setCursor(30, 90);
    tft.print(info.humidity);
    tft.println("%");

    tft.setCursor(10, 116);
    //tft.print("Opis: ");
    tft.println(info.description);

    tft.setCursor(10, 150);
    tft.print("Wsch: ");
    tft.println(formatUnixTime(info.sunrise));
    tft.setCursor(10, 176);
    tft.print("Zach: ");
    tft.println(formatUnixTime(info.sunset));

    tft.setCursor(10, 202);
    tft.print("Czujnik: ");
    tft.print(sensorTemp, 1);
    tft.println(" C");

String d = toLowerStr(info.description);

    if (d.indexOf("deszcz") >= 0 || d.indexOf("rain") >= 0) {
        drawRainIcon(160, 40, ST77XX_CYAN);
    } 
    else if (d.indexOf("snieg") >= 0 || d.indexOf("snow") >= 0) {
        drawSnowIcon(160, 40, ST77XX_CYAN);
    } 
    else if (d.indexOf("bezchmurnie") >= 0 || d.indexOf("clear") >= 0) {
        drawSunIcon(160, 40, ST77XX_YELLOW);             
    }
    else if (d.indexOf("few clouds") >= 0 || d.indexOf("scattered clouds") >= 0) {
        drawSunWithCloudIcon(160, 40, ST77XX_YELLOW);   
    } 
    else if (d.indexOf("broken clouds") >= 0 || d.indexOf("overcast") >= 0 || 
         d.indexOf("chmury") >= 0 || d.indexOf("cloud") >= 0) {
            drawCloudIcon(160, 40, ST77XX_WHITE);           
    } 
    
    else {
        drawSunIcon(160, 40, ST77XX_YELLOW);            
    }

    tft.drawLine(0, 240, 240, 240, ST77XX_WHITE);

}

void showForecastRow(const ForecastDay& day) {
    tft.fillRect(0, 242, 240, 78, ST77XX_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(10, 250);
    tft.print(day.dayName);

    tft.setCursor(100, 250);
    tft.print(int(day.tempMin));
    tft.print("/");
    tft.print(int(day.tempMax));
    tft.print(" C");

    tft.setCursor(10, 280);
    tft.print(day.description);

    String desc = day.description;
    desc.toLowerCase();

    if (desc.indexOf("rain") >= 0 || desc.indexOf("deszcz") >= 0) {
        drawRainIcon(200, 250, ST77XX_CYAN);
    } 
    else if (desc.indexOf("snow") >= 0 || desc.indexOf("snieg") >= 0) {
        drawSnowIcon(200, 250, ST77XX_CYAN);
    } 
    else if (desc.indexOf("cloud") >= 0 || desc.indexOf("chmur") >= 0) {
        drawCloudIcon(190, 245, ST77XX_WHITE);
    } 
    else if (desc.indexOf("storm") >= 0 || desc.indexOf("burz") >= 0) {
        drawCloudIcon(190, 245, ST77XX_WHITE);
        drawRainIcon(190, 260, ST77XX_YELLOW);
    } 
    else {
        drawSunIcon(200, 250, ST77XX_YELLOW);
    }
}

