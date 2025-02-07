#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <time.h>
#define emptyString String()

// NodeMCU sensor pin definition
#define LIGHTSENSOR1 A0
#define LED_PIN D1 // LED connected to GPIO5 (D1)

// Error handling functions
#include "errors.h"
// Configuration data
#include "configuration.h"

// Define MQTT port
const int MQTT_PORT = 8883;

// Define subscription and publication topics
const char MQTT_SUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";
const char MQTT_PUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";

// Enable or disable summer-time
#ifdef USE_SUMMER_TIME_DST
uint8_t DST = 1;
#else
uint8_t DST = 0;
#endif

// Create Transport Layer Security (TLS) connection
WiFiClientSecure net;
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

// Initialize MQTT client
MQTTClient client;

unsigned long lastMs = 0;
time_t now;
time_t nowish = 1510592825;

// Track last received value
int lastReceivedValue = -1;

// NTP Time synchronization
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

// Handle incoming MQTT messages
void messageReceived(String &topic, String &payload) {
  Serial.println("Received [" + topic + "]: " + payload);

  // Parse incoming JSON payload
  DynamicJsonDocument jsonDoc(256);
  DeserializationError error = deserializeJson(jsonDoc, payload);
  if (error) {
    Serial.println("Error parsing JSON!");
    return;
  }

  // Extract the value from the payload
  int newValue = jsonDoc["state"]["reported"]["value"] | -1;

  // Check if the new value is different from the last value
  if (newValue != -1 && newValue != lastReceivedValue) {
    Serial.print("New value: ");
    Serial.println(newValue);

    // Update the last received value
    lastReceivedValue = newValue;

    // Indicate new data with LED
    digitalWrite(LED_PIN, HIGH); // Turn LED ON
    delay(500);                  // LED ON for 500ms
    digitalWrite(LED_PIN, LOW);  // Turn LED OFF
  }
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
  DynamicJsonDocument jsonBuffer(JSON_OBJECT_SIZE(3) + 100);
  JsonObject root = jsonBuffer.to<JsonObject>();
  JsonObject state = root.createNestedObject("state");
  JsonObject state_reported = state.createNestedObject("reported");

  // Read the light sensor value
  int value1 = analogRead(LIGHTSENSOR1);
  state_reported["value"] = value1;

  Serial.printf("Sending [%s]: ", MQTT_PUB_TOPIC);
  serializeJson(root, Serial);
  Serial.println();

  char shadow[measureJson(root) + 1];
  serializeJson(root, shadow, sizeof(shadow));
  if (!client.publish(MQTT_PUB_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println();
  Serial.println();

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Connect to Wi-Fi and MQTT
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
}

void loop() {
  now = time(nullptr);
  if (!client.connected()) {
    verifyWiFiAndMQTT();
  } else {
    client.loop();

    // Send data periodically
    if (millis() - lastMs > 5000) {
      lastMs = millis();
      sendData();
    }
  }
}
