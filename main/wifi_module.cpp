#include "wifi_module.h"

WiFiModule::WiFiModule(const char* ssid, const char* password)
    : _ssid(ssid), _password(password) {}

void WiFiModule::begin() {
    connect();
}

void WiFiModule::connect() {
    WiFi.begin(_ssid, _password);
    Serial.print("Connessione WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connesso!");
}
