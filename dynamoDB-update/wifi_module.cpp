#include "wifi_module.h"

WiFiModule::WiFiModule(const char* ssid, const char* password, const char* client_id)
    : _ssid(ssid), _password(password), _client_id(client_id) {}

void WiFiModule::begin() {
    connect();
}

void WiFiModule::connect() {
    WiFi.hostname(_client_id);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);
    Serial.print("Connessione WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connesso!");
}

wl_status_t WiFiModule::status() {
  return WiFi.status();
}