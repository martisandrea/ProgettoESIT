#include "wifi_module.h"
#include "mqtt_module.h"
#include "sensor_module.h"
#include "time_module.h"

#include "errors.h"         // Error handling functions
#include "configuration.h"  // Configuration data

#define EMPTY_STRING String()
#define DEVICE_ID "Sensor1"

// MQTT datas
const int MQTT_PORT = 1883;
//const char* CLIENT_ID = "NodeMCU_1234";
const char* MQTT_TOPIC = "sensor/data";
const int TIME_TABLE = 3600;

WiFiModule wifiModule(ssid, password);
MQTTClient mqttClient;
WiFiClientSecure net;
MQTTModule mqttModule(mqttClient, MQTT_HOST, MQTT_PORT, net);
SensorModule sensor;
TimeModule timeModule(TIME_TABLE);

unsigned long lastMsg = 0;

// MQTT management of outgoing messages
void sendData(void) {
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
    lwMQTTErr(client.lastError());
}


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
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();
    sendData();
  }
}
