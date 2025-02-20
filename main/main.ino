#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "configuration.h"  // Configuration data

#include "wifi_module.h"
#include "mqtt_module.h"
#include "sensor_module.h"
#include "time_module.h"

#define EMPTY_STRING String()
#define DEVICE_ID "Sensor1"

// MQTT data
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "sensor/data";
// TimeModule data
const int TIME_TABLE = 3600;

WiFiModule wifiModule(ssid, password, THINGNAME);

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
MQTTClient mqttClient;
MQTTModule mqttModule(mqttClient, MQTT_HOST, MQTT_PORT, THINGNAME, MQTT_TOPIC);

SensorModule sensor;

TimeModule timeModule(TIME_TABLE);

unsigned long lastMsg = 0;

// MQTT management of outgoing messages
void sendData() {
  JsonDocument doc;
  doc.to<JsonObject>();
  JsonObject state = doc["state"].to<JsonObject>();
  JsonObject state_reported = state["reported"].to<JsonObject>();

  sensor.readSensor();
  timeModule.updateTime();

  String date_time = timeModule.getFormattedDate();
  date_time = date_time + " ";
  date_time = date_time + timeModule.getFormattedTime();

  float avgT = sensor.getTemperature();
  float avgH = sensor.getHumidity();

  state_reported["sensor_data_pk"] = date_time + " " + DEVICE_ID;
  state_reported["device_id"] = DEVICE_ID;
  state_reported["time_stamp"] = date_time;
  state_reported["temperature"] = avgT;
  state_reported["humidity"] = avgH;

  Serial.printf("Sending [%s]: ", MQTT_TOPIC);
  serializeJson(doc, Serial);
  Serial.println();
  char shadow[measureJson(doc) + 1];
  serializeJson(doc, shadow, sizeof(shadow));
  if (!mqttModule.publish(MQTT_TOPIC, "prova", false, 0))
    lwMQTTErr(mqttModule.lastError());
}


void setup() {
  Serial.begin(115200);

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  // Inizializzazione dei moduli
  wifiModule.begin();
  mqttModule.begin(net);
  sensor.begin();
  timeModule.begin();
}

void loop() {
  mqttModule.loop();
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    sendData();
  }
}
