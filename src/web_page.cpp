#include <WiFi.h>
#include <DHT.h>
#include "web_page.h"
#include "wifi_setup.h"

#define HISTORY_SIZE 60
float tempHistory[HISTORY_SIZE];
float humHistory[HISTORY_SIZE];
unsigned long timeHistory[HISTORY_SIZE];

int historyIndex = 0;
bool bufferFilled = false;

WiFiServer server(80);

void initWebServer() {
    server.begin();
    Serial.println("WebServer uruchomiony na porcie 80");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void saveDataPoint(float t, float h) {
    tempHistory[historyIndex] = t;
    humHistory[historyIndex] = h;
    timeHistory[historyIndex] = millis();
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
    if (historyIndex == 0) bufferFilled = true;
}

void handleWebServer(float Temp, float Humidity) {
    WiFiClient client = server.available();
    if (!client) return;

    String currentLine = "";
    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            if (c == '\n') {
                if (currentLine.length() == 0) {

                    float t = Temp;
                    float h = Humidity;

                    String html = "HTTP/1.1 200 OK\r\n";
                    html += "Content-Type: text/html\r\n";
                    html += "Connection: close\r\n\r\n";
                    html += "<!DOCTYPE HTML><html><head>";
                    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
                    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
                    html += "<style>";
                    html += "body{font-family:Arial;background:#f4f4f4;text-align:center;margin:0;padding:0}";
                    html += ".container{display:flex;flex-direction:column;align-items:center;gap:20px;margin-top:20px}";
                    html += ".box{background:#fff;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);width:90%;max-width:600px}";
                    html += "h1{color:#333}";
                    html += ".gauge{width:250px;height:250px;margin:20px auto}";
                    html += ".value{font-size:22px;font-weight:bold;margin:10px 0}";
                    html += ".footer{margin-top:20px;color:#777;font-size:14px}";
                    html += "canvas{max-width:600px;}";
                    html += "</style></head><body>";

                    html += "<div class='container'>";

                    html += "<div class='box'>";
                    html += "<h1>ESP32 Weather Station</h1>";
                    html += "<h2>Temp</h2>";
                    html += "<div class='gauge'><canvas id='tempGauge'></canvas></div>";
                    html += "<div class='value' style='color:red'>" + String(t, 1) + " C</div>";
                    html += "<h2>Humidity</h2>";
                    html += "<div class='gauge'><canvas id='humGauge'></canvas></div>";
                    html += "<div class='value' style='color:blue'>" + String(h, 1) + " %</div>";
                    html += "</div>";


                    html += "<div class='box'>";
                    html += "<h2>History (last 5h)</h2>";
                    html += "<canvas id='historyChart'></canvas>";
                    html += "</div>";

                    html += "</div>"; 

                    html += "<div class='footer'>Dane z czujnika DHT11</div>";

                    html += "<script>";

                    html += "function gauge(id,val,max,label,color){";
                    html += "var ctx=document.getElementById(id).getContext('2d');";
                    html += "new Chart(ctx,{type:'doughnut',data:{datasets:[{data:[val,max-val],backgroundColor:[color,'#ddd'],borderWidth:0}],labels:[label,'']},"; 
                    html += "options:{responsive:true,cutout:'70%',plugins:{legend:{display:false},tooltip:{enabled:false}}}});} ";

                    html += "gauge('tempGauge'," + String(t,1) + ",50,'Temp','red');";
                    html += "gauge('humGauge'," + String(h,1) + ",100,'Hum','blue');";

                    int count = bufferFilled ? HISTORY_SIZE : historyIndex;

                    html += "const labels=[";
                    for (int i = 0; i < count; i++) {
                        int idx = (historyIndex + i) % count;
                        html += "'" + String(i) + "',";
                    }
                    html += "];";

                    html += "const tempData=[";
                    for (int i = 0; i < count; i++) {
                        int idx = (historyIndex + i) % count;
                        html += String(tempHistory[idx],1) + ",";
                    }
                    html += "];";

                    html += "const humData=[";
                    for (int i = 0; i < count; i++) {
                        int idx = (historyIndex + i) % count;
                        html += String(humHistory[idx],1) + ",";
                    }
                    html += "];";

                    html += "var ctxH=document.getElementById('historyChart').getContext('2d');";
                    html += "new Chart(ctxH,{";
                    html += "type:'line',";
                    html += "data:{labels:labels,datasets:[";
                    html += "{label:'Temp (C)',data:tempData,borderColor:'red',backgroundColor:'rgba(255,0,0,0.1)',fill:true,tension:0.3},";
                    html += "{label:'Hum(%)',data:humData,borderColor:'blue',backgroundColor:'rgba(0,0,255,0.1)',fill:true,tension:0.3}";
                    html += "]},"; 
                    html += "options:{responsive:true,scales:{y:{beginAtZero:true}},plugins:{legend:{display:true}}}";
                    html += "});";

                    html += "</script></body></html>";

                    client.println(html);
                    break;
                } else {
                    currentLine = "";
                }
            } else if (c != '\r') {
                currentLine += c;
            }
        }
    }
    delay(1);
    client.stop();
}
