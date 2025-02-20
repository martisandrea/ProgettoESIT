#include "mqtt_module.h"

MQTTModule::MQTTModule(MQTTClient& client, const char* mqttHost, int mqttPort, const char* clientId, const char* mqttTopic)
  : _mqttClient(client), _mqttHost(mqttHost), _mqttPort(mqttPort), _clientId(clientId), _mqttTopic(mqttTopic) {}

void MQTTModule::begin(WiFiClientSecure net) {
  _net = net;
  _mqttClient.begin(_mqttHost, _mqttPort, net);
  connect();
}

void MQTTModule::loop() {
  if (!_mqttClient.connected()) {
    connect();
  }
  _mqttClient.loop();
}

bool MQTTModule::publish(const char* topic, const char* payload) {
  return _mqttClient.publish(topic, payload);
}

bool MQTTModule::publish(const char* topic, const char* payload, bool retained, int qos) {
  return _mqttClient.publish(topic, payload, retained, qos);
}

void MQTTModule::connect(bool nonBlocking) {
  while (!_mqttClient.connected()) {
    Serial.println("TEST CONNESSIONE " + String(_clientId) + " " + String(_mqttClient.connect(_clientId)));
    if (_mqttClient.connect(_clientId)) {
      Serial.println("connected!");
      if (!_mqttClient.subscribe(_mqttTopic))
        lwMQTTErr(_mqttClient.lastError());
    } else {
      Serial.print("SSL Error Code: ");
      Serial.println(_net.getLastSSLError());
      Serial.print("failed, reason -> ");
      lwMQTTErrConnection(_mqttClient.returnCode());
      if (!nonBlocking) {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      } else {
        Serial.println(" <");
      }
    }
  }
}

lwmqtt_err_t MQTTModule::lastError() {
  return _mqttClient.lastError();
}
