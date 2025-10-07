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
#include "web_page.h"

#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


float localTemp = 0.0;
float localHumidity = 0.0;

unsigned long lastUpdate = 0;
unsigned long lastForecastChange = 0;

const unsigned long updateInterval = 600000;   
const unsigned long forecastInterval = 5000;   
const unsigned long historyInterval = 5UL * 60UL * 1000UL; 
unsigned long lastHistorySave = 0;

WeatherInfo currentWeather;
std::vector<ForecastDay> forecast;
size_t currentDay = 0;

void setup() {
    Serial.begin(115200);
    connectToWiFi(WIFI_SSID, WIFI_PASS);
    pinMode(2, OUTPUT);
    initDisplay();
    initWebServer();  
    
    Serial.println("Wypełniam tablicę historią testową...");
    for (int i = 0; i < 60; i++) {
        float fakeT = 22.0 + random(-15, 15) / 10.0;  
        float fakeH = 45.0 + random(-20, 20) / 10.0;  
        saveDataPoint(fakeT, fakeH);
    }
    Serial.println("Historia startowa załadowana.");

    dht.begin();
    delay(1000);

    Serial.println("Pobieram dane pogodowe...");
    String currentData = getWeatherData(API_KEY, CITY, COUNTRY);
    currentWeather = parseWeatherData(currentData);

    localTemp = dht.readTemperature();
    localHumidity = dht.readHumidity();

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
        float h = dht.readHumidity();   
        if (!isnan(t)) {
            localTemp = t;
        }
        if (!isnan(h)) {
            localHumidity = h;
        }
    }

    if (now - lastHistorySave >= historyInterval) {
    lastHistorySave = now;
    saveDataPoint(localTemp, localHumidity);
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

    handleWebServer(localTemp, localHumidity);
}
