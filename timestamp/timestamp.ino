#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Dati di accesso alla rete Wi-Fi
const char* ssid = "iPhone di Riccardo";
const char* password = "passw115";

int countTime = 0;

// Impostazione del fuso orario (esempio per l'Italia: UTC+1)
const long utcOffsetInSeconds = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione in corso...");
  }
  Serial.println("Connesso alla rete Wi-Fi");

  timeClient.begin();

}

void loop() {
  int startTime = millis();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
    countTime = countTime +1;
  } else {
    Serial.println("");
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    int endTime = millis();
    Serial.println("time difference: " + String(countTime)));
    countTime=0;
    delay(1000);
    
  }

}
