#include <WiFi.h>
#include "wifi_setup.h"

void connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Łączenie z WiFi ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nPołączono z WiFi!");
    Serial.print("Adres IP: ");
    Serial.println(WiFi.localIP());
}
