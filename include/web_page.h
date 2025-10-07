#ifndef WEB_PAGE_H
#define WEB_PAGE_H

void initWebServer();
void handleWebServer(float localTemp,float localHumidity);
void saveDataPoint(float temperature, float humidity);

#endif
