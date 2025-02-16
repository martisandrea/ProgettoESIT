#include "wifi_module.h"
#include "mqtt_module.h"
#include "sensor_module.h"
#include "time_module.h"
#include "configuration.h"

// MQTT datas
const char* mqttServer = "broker.mqtt-dashboard.com";
const int mqttPort = 1883;
const char* clientId = "NodeMCU_1234";
const char* mqttTopic = "sensor/data";
const int timeTable = 3600;

WiFiModule wifiModule(ssid, password);
WiFiClient espClient;  // Necessario per il client MQTT
MQTTModule mqttModule(espClient, mqttServer, mqttPort, clientId);
SensorModule sensor;
TimeModule timeModule(timeTable);

void setup() {
  Serial.begin(115200);

  // Inizializzazione dei moduli
  wifiModule.begin();
  mqttModule.begin();
  sensor.begin();
  timeModule.begin();
}

void loop() {
  mqttModule.loop();
  sensor.readSensor();
  timeModule.updateTime();

  String date = timeModule.getFormattedDate();
  date = date + " ";
  date = date + timeModule.getFormattedTime();

  String temperature = String(sensor.getTemperature());
  String humidity = String(sensor.getHumidity());

  // Crea un payload in formato JSON con i dati del sensore e il timestamp
  String payload = "{";
  payload += "\"date\":\"" + date + "\",";
  payload += "\"temperature\":" + temperature + ",";
  payload += "\"humidity\":" + humidity;
  payload += "}";

  // Pubblica i dati via MQTT
  mqttModule.publish(mqttTopic, payload.c_str());

  Serial.println(payload);
  delay(2000);
}
