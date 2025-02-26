#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <DHT.h>
#include <Arduino.h>

// Definizioni per il DHT11
#define DHTPIN D4
#define DHTTYPE DHT11

// Parametri per il calcolo della media
const int maxValues = 10;

class SensorModule {
  public:
    SensorModule();
    void begin();
    void readSensor();
    float getTemperature(); // Ritorna la temperatura media
    float getHumidity();    // Ritorna l'umidità media

  private:
    DHT dht;
    float temperValues[maxValues];
    float humValues[maxValues];
    int counterValues;
    float sumT;
    float sumH;
    float avgT;
    float avgH;
};

#endif
