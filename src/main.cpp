#include "wifi_setup.h"
#include "api_client.h"
#include "json_parser.h"
#include "C:\Users\IGOR\OneDrive\Dokumenty\PlatformIO\Projects\smart-weather-station\config\secrets.h"
#include "C:\Users\IGOR\OneDrive\Dokumenty\PlatformIO\Projects\smart-weather-station\config\settings.h"
#include <Arduino.h>
#include <SPI.h>
unsigned long lastUpdate = 0;       // czas ostatniego odświeżenia
const unsigned long updateInterval = 600000;  // co 60s (1 minuta)

void setup() {
    Serial.begin(115200);
    connectToWiFi(WIFI_SSID, WIFI_PASS);
    pinMode(2, OUTPUT);
}

void loop() {
    unsigned long now = millis();

    if (now - lastUpdate >= updateInterval) {
        lastUpdate = now;
        digitalWrite(2, HIGH);

        // === Aktualna pogoda ===
        String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
        WeatherInfo info = parseWeatherData(currentData);

        Serial.println("==== Aktualna pogoda ====");
        Serial.print("Temperatura: ");
        Serial.println(info.temp);
        Serial.print("Wilgotność: ");
        Serial.println(info.humidity);
        Serial.print("Opis: ");
        Serial.println(info.description);
        Serial.print("Wschód słońca: ");
        Serial.println(formatUnixTime(info.sunrise));
        Serial.print("Zachód słońca: ");
        Serial.println(formatUnixTime(info.sunset));
        Serial.println("=========================");

        // === Prognoza 7-dniowa ===
        Serial.println("==== Prognoza na 7 dni ====");
        String forecastData = getWeatherForecast(API_KEY, LAT, LON);
        parseForecast(forecastData);
        Serial.println("===========================");
        digitalWrite(2, LOW);
    }

}
