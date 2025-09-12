#include "json_parser.h"
#include <ArduinoJson.h>

WeatherInfo parseWeatherData(const String& json) {
    WeatherInfo info;
    DynamicJsonDocument doc(4096);

    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        // Zwracamy pustą strukturę
        info.temp = 0;
        info.humidity = 0;
        info.description = "Błąd";
        info.sunrise = 0;
        info.sunset = 0;
        return info;
    }

    info.temp = doc["main"]["temp"] | 0.0;
    info.humidity = doc["main"]["humidity"] | 0;
    info.description = doc["weather"][0]["description"].as<String>();
    info.sunrise = doc["sys"]["sunrise"] | 0;
    info.sunset  = doc["sys"]["sunset"] | 0;

    return info;
}

size_t parseForecast(const String& json, ForecastDay forecast[], size_t maxDays) {
    DynamicJsonDocument doc(32768);

    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        return 0;
    }

    JsonArray daily = doc["daily"];
    if (daily.isNull()) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < daily.size() && i < maxDays; i++) {
        JsonObject day = daily[i];
        forecast[count].tempDay = day["temp"]["day"] | 0.0;
        forecast[count].tempMin = day["temp"]["min"] | 0.0;
        forecast[count].tempMax = day["temp"]["max"] | 0.0;
        forecast[count].humidity = day["humidity"] | 0;
        forecast[count].description = day["weather"][0]["description"].as<String>();
        count++;
    }

    return count;
}

String formatUnixTime(unsigned long timestamp) {
    timestamp += 2 * 3600;  // korekta dla strefy
    time_t rawTime = (time_t)timestamp;
    struct tm *ti = gmtime(&rawTime);

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", ti->tm_hour, ti->tm_min);

    return String(buffer);
}
