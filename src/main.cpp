#include <Arduino.h>
#include "wifi_setup.h"
#include "api_client.h"
#include "json_parser.h"
#include "display.h"
#include <vector>
#include "secrets.h"
#include "settings.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float localTemp = 0.0;

unsigned long lastUpdate = 0;
unsigned long lastForecastChange = 0;

const unsigned long updateInterval = 600000;   // co 10 min pobieranie z API
const unsigned long forecastInterval = 5000;   // co 5 s zmiana dnia prognozy

WeatherInfo currentWeather;
std::vector<ForecastDay> forecast;
size_t currentDay = 0;

void setup() {
    Serial.begin(115200);
    connectToWiFi(WIFI_SSID, WIFI_PASS);
    pinMode(2, OUTPUT);
    initDisplay();

    dht.begin();
    delay(1000);

    Serial.println("Pobieram dane pogodowe...");
    String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
    currentWeather = parseWeatherData(currentData);

    localTemp = dht.readTemperature();

    String forecastData = getWeatherForecast(API_KEY, LAT, LON);
    forecast = parseForecast(forecastData);

    showCurrentWeather(currentWeather,localTemp); 
    if (!forecast.empty()) {
        showForecastRow(forecast[currentDay]); 
    }

    lastUpdate = millis();
    lastForecastChange = millis();
}

void loop() {
    unsigned long now = millis();

    static unsigned long lastSensorRead = 0;
    if (now - lastSensorRead > 5000) {
        lastSensorRead = now;
        float t = dht.readTemperature();
        if (!isnan(t)) {
            localTemp = t;
        }
    }

    if (now - lastUpdate >= updateInterval) {
        lastUpdate = now;

        Serial.println(" Aktualizacja danych...");
        String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
        currentWeather = parseWeatherData(currentData);

        String forecastData = getWeatherForecast(API_KEY, LAT, LON);
        forecast = parseForecast(forecastData);

        showCurrentWeather(currentWeather, localTemp);
        currentDay = 0;
        showForecastRow(forecast[currentDay]);
    }

    if (!forecast.empty() && (now - lastForecastChange >= forecastInterval)) {
        lastForecastChange = now;
        currentDay = (currentDay + 1) % forecast.size();
        showForecastRow(forecast[currentDay]);
    }
}
