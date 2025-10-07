#include <WiFi.h>
#include <HTTPClient.h>
#include "api_client.h"

String getWeatherData(const char* apiKey, const char* city, const char* country) {
    if (WiFi.status() != WL_CONNECTED) {
        return "{}"; 
    }

    HTTPClient http;
    String url = "https://api.openweathermap.org/data/2.5/weather?q=" 
                 + String(city) + "," + String(country) 
                 + "&appid=" + apiKey + "&units=metric&lang=pl";

    http.begin(url);
    int httpResponseCode = http.GET();
    String payload = "{}";

    if (httpResponseCode > 0) {
        payload = http.getString();
    } else {
        Serial.print("Błąd pobierania danych: ");
        Serial.println(httpResponseCode);
    }

    http.end();
    return payload;
}

String getWeatherForecast(const char* apiKey, float lat, float lon) {
    if (WiFi.status() != WL_CONNECTED) {
        return "{}"; 
    }

    HTTPClient http;
    String url = "https://api.openweathermap.org/data/3.0/onecall?lat=" + String(lat) + "&lon=" + String(lon) + "&exclude=minutely,alerts&units=metric&appid=" + String(apiKey);

    http.begin(url);
    int httpResponseCode = http.GET();
    String payload = "{}";

    if (httpResponseCode > 0) {
        payload = http.getString();
    } else {
        Serial.print("Błąd pobierania prognozy: ");
        Serial.println(httpResponseCode);
    }

    http.end();
    return payload;
}
