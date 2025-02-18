#include "mqtt_module.h"

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

MQTTModule::MQTTModule(MQTTClient& client, const char* mqttHost, int mqttPort, WiFiClientSecure net)
    : _mqttClient(client), _mqttHost(mqttHost), _mqttPort(mqttPort), _net(net) {}

void MQTTModule::begin() {
    _net.setTrustAnchors(&cert);
    _net.setClientRSACert(&client_crt, &key);
    _mqttClient.begin(_mqttHost, _mqttPort, _net);
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

bool MQTTModule::publish(const char* topic, const char* payload, bool retained, int qos) {
    return _mqttClient.publish(topic, payload, retained, qos);
}

void MQTTModule::reconnect(bool nonBlocking = false) {
    while (!_mqttClient.connected()) {
    if (_mqttClient.connect(THINGNAME)) {
      Serial.println("connected!");
      if (!_mqttClient.subscribe(MQTT_SUB_TOPIC))
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
