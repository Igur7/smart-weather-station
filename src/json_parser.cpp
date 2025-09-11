#include "json_parser.h"
#include <ArduinoJson.h>

WeatherInfo parseWeatherData(const String& json) {
    WeatherInfo info;
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, json);

    info.temp = doc["main"]["temp"] | 0.0;
    info.humidity = doc["main"]["humidity"] | 0;
    info.description = doc["weather"][0]["description"].as<String>();
    info.sunrise = doc["sys"]["sunrise"] | 0;
    info.sunset  = doc["sys"]["sunset"] | 0;
    

    return info;
}

void parseForecast(const String& json) {
    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.print("Błąd deserializacji JSON: ");
        Serial.println(error.c_str());
        return;
    }

    JsonArray daily = doc["daily"];
    if (daily.isNull()) {
        Serial.println("Brak danych 'daily' w JSON!");
        return;
    }

    for (size_t i = 0; i < daily.size() && i < 7; i++) {
        JsonObject day = daily[i];
        float tempDay = day["temp"]["day"] | 0.0;
        float tempMin = day["temp"]["min"] | 0.0;
        float tempMax = day["temp"]["max"] | 0.0;
        int humidity  = day["humidity"] | 0;
        String description = day["weather"][0]["description"].as<String>();

        Serial.print("Dzień "); Serial.println(i + 1);
        Serial.print("  Temp (dzień): "); Serial.println(tempDay);
        Serial.print("  Min: "); Serial.println(tempMin);
        Serial.print("  Max: "); Serial.println(tempMax);
        Serial.print("  Wilgotność: "); Serial.println(humidity);
        Serial.print("  Opis: "); Serial.println(description);
        Serial.println("--------------------");
    }
}

String formatUnixTime(unsigned long timestamp) {

    timestamp += 2 * 3600;  

    time_t rawTime = (time_t)timestamp;
    struct tm *ti = gmtime(&rawTime); 

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", ti->tm_hour, ti->tm_min);

    return String(buffer);
}
