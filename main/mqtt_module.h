#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#include <PubSubClient.h>
#include <ESP8266WiFi.h>  // Necessario per WiFiClient

class MQTTModule {
public:
    MQTTModule(WiFiClient& client, const char* mqttServer, int mqttPort, const char* clientId);
    void begin();
    void loop();
    bool publish(const char* topic, const char* payload);
    
private:
    void reconnect();
    PubSubClient _mqttClient;
    const char* _mqttServer;
    int _mqttPort;
    const char* _clientId;
};

#endif
