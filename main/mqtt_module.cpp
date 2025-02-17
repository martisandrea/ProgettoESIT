#include "mqtt_module.h"

MQTTModule::MQTTModule(WiFiClient& client, const char* mqttServer, int mqttPort, const char* clientId)
    : _mqttClient(client), _mqttServer(mqttServer), _mqttPort(mqttPort), _clientId(clientId) {}

void MQTTModule::begin() {
    _mqttClient.setServer(_mqttServer, _mqttPort);
}

void MQTTModule::loop() {
    if (!_mqttClient.connected()) {
        reconnect();
    }
    _mqttClient.loop();
}

bool MQTTModule::publish(const char* topic, const char* payload) {
    return _mqttClient.publish(topic, payload);
}

void MQTTModule::reconnect() {
    while (!_mqttClient.connected()) {
        Serial.print("Connessione a MQTT...");
        if (_mqttClient.connect(_clientId)) {
            Serial.println("connesso");
        } else {
            Serial.print("fallita, rc=");
            Serial.println(_mqttClient.state());
            delay(5000);
        }
    }
}
