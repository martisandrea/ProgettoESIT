#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#include <PubSubClient.h>
#include <MQTT.h>
#include <ESP8266WiFi.h>  // Necessario per WiFiClient
#include <WiFiClientSecure.h>
#include "configuration.h"

class MQTTModule {
public:
    MQTTModule(MQTTClient& client, const char* mqttHost, int mqttPort, WiFiClientSecure net);
    void begin();
    void loop();
    bool publish(const char* topic, const char* payload);
    bool publish(const char* topic, const char* payload, bool retained, int qos);
    
private:
    void reconnect();
    MQTTClient _mqttClient;
    const char* _mqttHost;
    int _mqttPort;
    WiFiClientSecure _net;
};

#endif
