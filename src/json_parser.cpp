#include "json_parser.h"
#include <ArduinoJson.h>
#include <time.h>

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

String getWeekdayShort(time_t t) {
    struct tm *ti = localtime(&t);
    switch (ti->tm_wday) {
        case 0: return "Niedzila";
        case 1: return "Poniedzialek";
        case 2: return "Wtorek";
        case 3: return "Srod";
        case 4: return "Czwartek";
        case 5: return "Piatek";
        case 6: return "Sobota";
    }
    return "";
}

std::vector<ForecastDay> parseForecast(const String& json) {
    std::vector<ForecastDay> forecast;
    DynamicJsonDocument doc(32768);

    if (deserializeJson(doc, json)) {
        return forecast; // pusty wektor przy błędzie
    }

    JsonArray daily = doc["daily"];
    if (daily.isNull()) return forecast;

for (size_t i = 1; i < daily.size(); i++) {  
    JsonObject day = daily[i];
    ForecastDay f;
    time_t dt = day["dt"] | 0;
    f.dayName = getWeekdayShort(dt);

    f.tempDay = day["temp"]["day"] | 0.0;
    f.tempMin = day["temp"]["min"] | 0.0;
    f.tempMax = day["temp"]["max"] | 0.0;
    f.humidity = day["humidity"] | 0;
    f.description = day["weather"][0]["description"].as<String>();

    forecast.push_back(f);
}

    return forecast;
}

String formatUnixTime(unsigned long timestamp) {
    timestamp += 2 * 3600; 
    time_t rawTime = (time_t)timestamp;
    struct tm *ti = gmtime(&rawTime);

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", ti->tm_hour, ti->tm_min);

    return String(buffer);
}
