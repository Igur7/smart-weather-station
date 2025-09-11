#include <WiFi.h>
#include <HTTPClient.h>
#include "api_client.h"

// Aktualna pogoda (endpoint: /weather)
String getWeatherData(const char* apiKey, const char* city, const char* country) {
    if (WiFi.status() != WL_CONNECTED) {
        return "{}";  // brak połączenia
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

// Prognoza pogody (endpoint: /onecall)
String getWeatherForecast(const char* apiKey, float lat, float lon) {
    if (WiFi.status() != WL_CONNECTED) {
        return "{}";  // brak połączenia
    }

    HTTPClient http;
    String url = "https://api.openweathermap.org/data/3.0/onecall?lat=52.2298&lon=21.0118&exclude=minutely,alerts&units=metric&appid=" + String(apiKey);

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
