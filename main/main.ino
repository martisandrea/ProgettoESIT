#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <LiquidCrystal_I2C.h>  // LCD

#include "configuration.h"  // Configuration data
#include "errors.h"         // Error handling functions
#include "wifi_module.h"
#include "sensor_module.h"
#include "time_module.h"

#define EMPTY_STRING String()
#define DEVICE_ID "S1"  // Device ID

// MQTT data
const int MQTT_PORT = 8883;                       // Define MQTT port
const char* MQTT_SENSORS_TOPIC = "sensors/data";  //"$aws/things/" THINGNAME "/shadow/update";
const char* MQTT_ALERTS_TOPIC = "sensor/alert";   //"$aws/things/" THINGNAME "/shadow/update";
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
// Wiring: SDA pin is connected to A4(D2) and SCL pin to A5(D1).
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
int lcdColumns = 16;
int lcdRows = 16;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, lcdColumns, lcdRows);

unsigned long lastMsg = 0;
unsigned long lastAlarm = 0;
time_t now;
time_t nowish = 1510592825;
int numAlarms = 0;
float avgT = 0;
float avgH = 0;

float temperatureThreshold = 50;  // Temperature threshold for critical alert
float humidityThreshold = 25;     // Humidity threshold for critical alert

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
void messageReceived(String& topic, String& payload) {
  Serial.println("Received [" + topic + "]: " + payload);
}

// MQTT Broker connection
void connectToMqtt(bool nonBlocking = false) {
  Serial.print("MQTT connecting ");
  while (!client.connected()) {
    if (client.connect(THINGNAME)) {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_SENSORS_TOPIC))
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

  timeModule.updateTime();
  String date_time = timeModule.getFormattedDate() + " " + timeModule.getFormattedTime();

  sensor.readSensor();
  avgT = round(sensor.getTemperature() * 100) / 100;
  avgH = round(sensor.getHumidity() * 100) / 100;

  // se la temperatura ha un valore anomalo avviso tramite bot
  if (avgT > temperatureThreshold && avgH < humidityThreshold) {
    if (millis() - lastAlarm == 300000) {
      Serial.println("Critical Readings!");
      lastAlarm = millis();
      numAlarms++;
      // Send alert to MQTT topic
      JsonObject alert = doc["alert"].to<JsonObject>();
      alert["alert_pk"] = date_time + "-" + DEVICE_ID;
      alert["temperature"] = avgT;
      alert["humidity"] = avgH;
      alert["state"] = "active";
      //alert["device_id"] = DEVICE_ID;
      //alert["time_stamp"] = date_time;
      char alertMessage[measureJson(doc) + 1];
      serializeJson(doc, alertMessage, sizeof(alertMessage));
      if (!client.publish(MQTT_ALERTS_TOPIC, alertMessage, false, 0))
        lwMQTTErr(client.lastError());
    }
  }

  state_reported["sensor_data_pk"] = date_time + "-" + DEVICE_ID;
  //state_reported["device_id"] = DEVICE_ID;
  //state_reported["time_stamp"] = date_time;
  state_reported["temperature"] = avgT;
  state_reported["humidity"] = avgH;

  Serial.printf("Sending [%s]: ", MQTT_SENSORS_TOPIC);
  serializeJson(doc, Serial);
  Serial.println();
  char shadow[measureJson(doc) + 1];
  serializeJson(doc, shadow, sizeof(shadow));
  if (!client.publish(MQTT_SENSORS_TOPIC, shadow, false, 0))
    lwMQTTErr(client.lastError());
}

// Function to scroll text on the LCD
// row: row number where the text will be displayed
// message: message to scroll
// delayTime: delay between each character shifting
// lcdColumns: number of columns of your LCD
void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i = 0; i < lcdColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.println();
  Serial.println();
  Serial.println("Trying to connect with SSID: " + String(ssid));
  lcd.setCursor(0, 0);
  lcd.print("Connessione in");
  lcd.setCursor(0, 1);
  lcd.print("corso...");
  wifiModule.begin();
  sensor.begin();
  timeModule.begin();
  NTPConnect();
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
  client.begin(MQTT_HOST, MQTT_PORT, net);
  client.onMessage(messageReceived);
  connectToMqtt();
  lcd.clear();
}

void loop() {
  now = time(nullptr);
  if (!client.connected()) {
    lcd.clear();
    while (wifiModule.status() != WL_CONNECTED) {
      lcd.setCursor(0, 0);
      lcd.print("Non connesso");
      sensor.readSensor();
      if (avgT > temperatureThreshold && avgH < humidityThreshold) {
        // TODO: aggiornare il sistema per inviare allarme quando si connette di nuovo
        lcd.setCursor(0, 1);
        scrollText(1, "LETTURA CRITICA NON INVIATA", 300, lcdColumns);
      } else {
        lcd.setCursor(0, 1);
        lcd.print("Dati nella norma");
      }
      Serial.print(".");
      delay(5000);
    }
    connectToMqtt();
    lcd.clear();
  } else {
    client.loop();
    if (millis() - lastMsg > 5000) {
      lastMsg = millis();
      sendData();
    }
    if (millis() - lastAlarm > 305000) {
      numAlarms = 0;
    }
    lcd.setCursor(0, 0);
    lcd.print("Allarmi ultimi 5");
    lcd.setCursor(0, 1);
    lcd.print("minuti: " + String(numAlarms));
  }
}
