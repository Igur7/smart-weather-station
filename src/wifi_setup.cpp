#include <WiFi.h>
#include <ESPmDNS.h>

void connectToWiFi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("pogoda-device");  // Ustaw nazwę hosta WiFi
  WiFi.begin(ssid, password);
  Serial.print("Łączenie z WiFi: ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPołączono. IP: " + WiFi.localIP().toString());

  if (!MDNS.begin("pogoda")) {               // <-- nazwa: pogoda.local
    Serial.println("błąd mDNS");
  } else {
    Serial.println("mDNS uruchomione: http://pogoda.local");
  }
}
