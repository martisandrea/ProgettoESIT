#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <time.h>
#include <DHT.h>

#include "errors.h"         // Error handling functions
#include "configuration.h"  // Configuration data

#define emptyString String()
#define LIGHTSENSOR1 A0      // NodeMCU sensor pin definition
const int MQTT_PORT = 8883;  // Define MQTT port
// Define subscription and publication topics (on thing shadow)
const char MQTT_SUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update/sensors/data";
const char MQTT_PUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update/sensors/data";

// Enable or disable summer-time
#ifdef USE_SUMMER_TIME_DST
uint8_t DST = 1;
#else
uint8_t DST = 0;
#endif

// Create Transport Layer Security (TLS) connection
WiFiClientSecure net;
// Load certificates
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
// Initialize MQTT client
MQTTClient client;

// VARIABILI PER IL CALCOLO DEI VALORI MEDI
const int maxValues = 10;
float temperValues[maxValues];
float humValues[maxValues];
int counterValues = 0;

float sumT = 0;
float avgT = 0;
float sumH = 0;
float avgH = 0;

// DHT11 Humidity and Temperature Sensor
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMs = 0;
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
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
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
      if (!client.subscribe(MQTT_SUB_TOPIC))
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
  if (init_str != emptyString)
    Serial.print(init_str);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  if (init_str != emptyString)
    Serial.println("ok!");
}

void verifyWiFiAndMQTT(void) {
  connectToWiFi("Checking WiFi");
  connectToMqtt();
}

unsigned long previousMillis = 0;
const long interval = 5000;

// MQTT management of outgoing messages
void sendData(void) {
  JsonDocument doc;
  doc.to<JsonObject>();
  JsonObject state = doc["state"].to<JsonObject>();
  JsonObject state_reported = state["reported"].to<JsonObject>();

  float temValue = dht.readTemperature();
  float humValue = dht.readHumidity();

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

  state_reported["temperature"] = avgT;
  state_reported["device_id"] = counterValues;
  state_reported["humidity"] = avgH;

  //Serial.printf("Sending [%s]: ", MQTT_PUB_TOPIC);
  serializeJson(doc, Serial);
  //Serial.println();
  char shadow[measureJson(doc) + 1];
  serializeJson(doc, shadow, sizeof(shadow));
  if (!client.publish(MQTT_PUB_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println();
  Serial.println();
  WiFi.hostname(THINGNAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  connectToWiFi(String("Trying to connect with SSID: ") + String(ssid));
  NTPConnect();
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
  client.begin(MQTT_HOST, MQTT_PORT, net);
  client.onMessage(messageReceived);
  connectToMqtt();
  // AVVIO LA LETTURA DEL SENSORE DI UMIDITA' E TEMPERATURA
  dht.begin();
}

void loop() {
  now = time(nullptr);
  if (!client.connected()) {
    verifyWiFiAndMQTT();
  } else {
    client.loop();
    if (millis() - lastMs > 2000) {
      lastMs = millis();
      sendData();
    }
  }
}