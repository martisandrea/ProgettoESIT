#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi Configuration
const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxx";
const int maxValues = 10;
float temperValues[maxValues];
float humValues[maxValues];
int counterValues = 0;

float sumT = 0;
float avgT = 0;
float sumH = 0;
float avgH = 0;

// MQTT Configuration
const char* mqtt_server = "broker.mqtt-dashboard.com";
const int mqtt_port = 1883;
const char* mqtt_topic_temp = "sensor/temperature";
const char* mqtt_topic_hum = "sensor/humidity";
const char* mqtt_topic_light = "sensor/light";

// Light sensor (photoresistor)
#define LIGHTSENSOR A0

// DHT11 Humidity and Temperature Sensor
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Configure serial
  Serial.begin(115200);

  // WiFi connection
  //setup_wifi();
  // Configurazione MQTT
  //client.setServer(mqtt_server, mqtt_port);

  // Inizializzazione DHT11

  dht.begin();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Attempt to reconnect to MQTT broker if disconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCU_1234")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    //reconnect();
  }
  //client.loop();
  // Read data from the light sensor
  float humValue = dht.readHumidity();
  float temValue = dht.readTemperature();
  //float l = analogRead(LIGHTSENSOR);

  sumH = sumH - humValues[counterValues % 10];
  humValues[counterValues % 10] = humValue;
  sumH = sumH + humValue;

  sumT = sumT - temperValues[counterValues % 10];
  temperValues[counterValues % 10] = temValue;
  sumT = sumT + temValue;

  counterValues = counterValues + 1;

  if (counterValues < maxValues) {
    avgT = sumT / counterValues;
    avgH = sumH / counterValues;
  } else {
    avgT = sumT / maxValues;
    avgH = sumH / maxValues;
  }

  Serial.println("Read Temperature: " + String(temValue));
  Serial.println("Average Temperature: " + String(avgT));
  Serial.println("Read Humidity: " + String(humValue));
  Serial.println("Average Humidity: " + String(avgH));

  delay(2000);

  // Validate sensor reading
  /* if (isnan(h) || isnan(t) || isnan(l)) {
    Serial.println("Sensor reading error");
    return;
  } */
  // Publish data to MQTT
  /* char tempStr[8];
  dtostrf(t, 1, 2, tempStr);
  client.publish(mqtt_topic_temp, tempStr);
  char humStr[8];
  dtostrf(h, 1, 2, humStr);
  client.publish(mqtt_topic_hum, humStr);
  char lightStr[8];
  dtostrf(l, 1, 2, lightStr);
  client.publish(mqtt_topic_light, lightStr);
  Serial.print("Temperature: ");
  Serial.print(tempStr);
  Serial.print(" °C, Humidity: ");
  Serial.print(humStr);
  Serial.print(" % Light: ");
  Serial.println(lightStr); */

  // Wait for a second before next reading
  delay(1000);
}