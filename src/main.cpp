#include <Arduino.h>
#include "wifi_setup.h"
#include "api_client.h"
#include "json_parser.h"
#include "display.h"
#include "C:\Users\IGOR\OneDrive\Dokumenty\PlatformIO\Projects\smart-weather-station\config\secrets.h"
#include "C:\Users\IGOR\OneDrive\Dokumenty\PlatformIO\Projects\smart-weather-station\config\settings.h"

unsigned long lastUpdate = 0;
unsigned long lastDayChange = 0;
const unsigned long updateInterval = 600000; // co 10 minut odświeżanie danych z API
const unsigned long dayInterval = 10000;     // co 10 sekund zmiana dnia prognozy

WeatherInfo currentWeather;
ForecastDay forecast[7];
size_t forecastCount = 0;
size_t currentDay = 0;

void setup() {
    Serial.begin(115200);

    connectToWiFi(WIFI_SSID, WIFI_PASS);

    initDisplay();

    pinMode(2, OUTPUT);

    Serial.println("📡 Pobieram dane pogodowe...");
    String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
    currentWeather = parseWeatherData(currentData);
    showCurrentWeather(currentWeather);

    String forecastData = getWeatherForecast(API_KEY, LAT, LON);
    forecastCount = parseForecast(forecastData, forecast, 7);

    currentDay = 0;
    if (forecastCount > 0) {
        showDayForecast(forecast[currentDay], currentDay);
    }

    lastUpdate = millis();
    lastDayChange = millis();
}

void loop() {
    unsigned long now = millis();

    if (now - lastUpdate >= updateInterval) {
        lastUpdate = now;
        digitalWrite(2, HIGH);

        Serial.println(" Aktualizacja danych...");

        String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
        currentWeather = parseWeatherData(currentData);

        String forecastData = getWeatherForecast(API_KEY, LAT, LON);
        forecastCount = parseForecast(forecastData, forecast, 7);

        currentDay = 0;
        lastDayChange = now;

        showCurrentWeather(currentWeather);

        digitalWrite(2, LOW);
    }

    if (forecastCount > 0 && (now - lastDayChange >= dayInterval)) {
        lastDayChange = now;

        if (currentDay == 0) {

            showCurrentWeather(currentWeather);
        } else {

            showDayForecast(forecast[currentDay - 1], currentDay - 1);
        }

        currentDay = (currentDay + 1) % (forecastCount + 1);
    }
}


