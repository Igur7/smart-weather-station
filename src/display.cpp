#include "display.h"
#include <ArduinoJson.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); 
    tft.init(240, 320);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
}

void showCurrentWeather(const WeatherInfo& info) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 20);
    tft.printf("Temp: %.1f C", info.temp);

    tft.setCursor(10, 50);
    tft.printf("Wilg: %d %%", info.humidity);

    tft.setCursor(10, 80);
    tft.print("Opis: ");
    tft.print(info.description);

    tft.setCursor(10, 110);
    tft.printf("Wschod: %s", formatUnixTime(info.sunrise).c_str());

    tft.setCursor(10, 140);
    tft.printf("Zachod: %s", formatUnixTime(info.sunset).c_str());
}

void showForecast(const String& json) {
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.println("Błąd forecast JSON");
        return;
    }

    JsonArray daily = doc["daily"];
    if (daily.isNull()) return;

    int y = 180;
    for (size_t i = 0; i < daily.size() && i < 3; i++) { // tylko 3 dni
        JsonObject day = daily[i];
        float tempDay = day["temp"]["day"] | 0.0;
        String desc = day["weather"][0]["description"].as<String>();

        tft.setCursor(10, y);
        tft.printf("D%d: %.1fC %s", (int)(i+1), tempDay, desc.c_str());
        y += 25;
    }
}
