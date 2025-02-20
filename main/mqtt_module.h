#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#include <PubSubClient.h>
#include <MQTT.h>
#include <ESP8266WiFi.h>  // Necessario per WiFiClient
#include "errors.h"

class MQTTModule {
public:
    MQTTModule(MQTTClient& client, const char* mqttHost, int mqttPort, const char* clientId, const char* mqttTopic);
    void begin(WiFiClientSecure net);
    void loop();
    bool publish(const char* topic, const char* payload);
    bool publish(const char* topic, const char* payload, bool retained, int qos);
    lwmqtt_err_t lastError();
    
private:
    void connect(bool nonBlocking = false);
    MQTTClient& _mqttClient;
    const char* _mqttHost;
    int _mqttPort;
    WiFiClientSecure _net;
    const char* _clientId;
    const char* _mqttTopic;
};

#endif

