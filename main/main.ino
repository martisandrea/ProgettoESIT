#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>

#include "configuration.h"  // Configuration data
#include "errors.h"         // Error handling functions
#include "wifi_module.h"
#include "sensor_module.h"
#include "time_module.h"

#define EMPTY_STRING String()
#define DEVICE_ID "S1"  // Device ID

// MQTT data
const int MQTT_PORT = 8883;  // Define MQTT port
const char* MQTT_TOPIC = "sensors/data";  //"$aws/things/" THINGNAME "/shadow/update";
//const char MQTT_PUB_TOPIC[] = "sensors/data";  //"$aws/things/" THINGNAME "/shadow/update";
// TimeModule data
const int TIME_TABLE = 3600;

// Enable or disable summer-time
#ifdef USE_SUMMER_TIME_DST
uint8_t DST = 1;
#else
uint8_t DST = 0;
#endif

WiFiModule wifiModule(ssid, password, THINGNAME);

// Create Transport Layer Security (TLS) connection
WiFiClientSecure net;

// Load certificates
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
// Initialize MQTT client
MQTTClient client;
SensorModule sensor;

TimeModule timeModule(TIME_TABLE);

unsigned long lastMsg = 0;
time_t now;
time_t nowish = 1510592825;

// Get time through Simple Network Time Protocol
void NTPConnect(void) {
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  Serial.print("Current time: ");
  timeModule.updateTime();
  String date_time = timeModule.getFormattedDate() + " " + timeModule.getFormattedTime();
  Serial.println(date_time);
}

// MQTT management of incoming messages
void messageReceived(String &topic, String &payload) {
  Serial.println("Received [" + topic + "]: " + payload);
}

// MQTT Broker connection
void connectToMqtt(bool nonBlocking = false) {
  Serial.print("MQTT connecting ");
  while (!client.connected()) {
    if (client.connect(THINGNAME)) {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_TOPIC))
        lwMQTTErr(client.lastError());
    } else {
      Serial.print("SSL Error Code: ");
      Serial.println(net.getLastSSLError());
      Serial.print("failed, reason -> ");
      lwMQTTErrConnection(client.returnCode());
      if (!nonBlocking) {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      } else {
        Serial.println(" <");
      }
    }
    if (nonBlocking) break;
  }
}

// Wi-Fi connection
void connectToWiFi(String init_str) {
  if (init_str != EMPTY_STRING)
    Serial.print(init_str);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  if (init_str != EMPTY_STRING)
    Serial.println("ok!");
}

unsigned long previousMillis = 0;
const long interval = 5000;

// MQTT management of outgoing messages
void sendData() {
  JsonDocument doc;
  doc.to<JsonObject>();
  JsonObject state = doc["state"].to<JsonObject>();
  JsonObject state_reported = state["reported"].to<JsonObject>();
  
  sensor.readSensor();
  timeModule.updateTime();

  String date_time = timeModule.getFormattedDate() + " " + timeModule.getFormattedTime();

  float avgT = round(sensor.getTemperature() * 100) / 100;
  float avgH = round(sensor.getHumidity() * 100) / 100;

  state_reported["sensor_data_pk"] = date_time + "-" + DEVICE_ID;
  state_reported["device_id"] = DEVICE_ID;
  state_reported["time_stamp"] = date_time;
  state_reported["temperature"] = avgT;
  state_reported["humidity"] = avgH;

  Serial.printf("Sending [%s]: ", MQTT_TOPIC);
  serializeJson(doc, Serial);
  Serial.println();
  char shadow[measureJson(doc) + 1];
  serializeJson(doc, shadow, sizeof(shadow));
  if (!client.publish(MQTT_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println();
  Serial.println("Trying to connect with SSID: " + String(ssid));
  wifiModule.begin();
  sensor.begin();
  timeModule.begin();
  NTPConnect();
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
  client.begin(MQTT_HOST, MQTT_PORT, net);
  client.onMessage(messageReceived);
  connectToMqtt();
}

void loop() {
  now = time(nullptr);
  if (!client.connected()) {
    while (wifiModule.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    connectToMqtt();
  } else {
    client.loop();
    if (millis() - lastMsg > 5000) {
      lastMsg = millis();
      sendData();
    }
  }
}
