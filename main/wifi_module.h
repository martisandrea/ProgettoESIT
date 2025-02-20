#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <ESP8266WiFi.h>

class WiFiModule {
public:
    WiFiModule(const char* ssid, const char* password, const char* client_id);
    void begin();
    
private:
    void connect();
    const char* _ssid;
    const char* _password;
    const char* _client_id;
};

#endif
