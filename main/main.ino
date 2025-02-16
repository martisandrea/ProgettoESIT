#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "sensor_module.h"
#include "time_module.h"

// Dati di accesso WiFi
const char* ssid = "iPhone di Riccardo";
const char* password = "passw115";

// Dati MQTT
const char* mqtt_server = "broker.mqtt-dashboard.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "sensor/data";

// Client MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Inizializza i moduli
SensorModule sensor;
TimeModule timeModule(3600); // UTC+1 per l'Italia

void setup() {
  Serial.begin(115200);
  
  // Connessione WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connessione WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connesso!");

  // Configurazione MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Inizializzazione dei moduli
  sensor.begin();
  timeModule.begin();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connessione a MQTT...");
    if (client.connect("NodeMCU_1234")) {
      Serial.println("connesso");
    } else {
      Serial.print("fallita, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Aggiorna le letture dal sensore e dal modulo tempo
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
  client.publish(mqtt_topic, payload.c_str());
  
  // Debug su Serial
  Serial.println(payload);
  
  delay(2000);
}
