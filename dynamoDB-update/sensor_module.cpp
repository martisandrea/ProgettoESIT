#include "sensor_module.h"

SensorModule::SensorModule()
  : dht(DHTPIN, DHTTYPE),
    counterValues(0),
    sumT(0), sumH(0),
    avgT(0), avgH(0) {
  // Inizializza gli array a zero
  for (int i = 0; i < maxValues; i++) {
    temperValues[i] = 0;
    humValues[i] = 0;
  }
}

void SensorModule::begin() {
  dht.begin();
}

void SensorModule::readSensor() {
  float humValue = dht.readHumidity();
  float temValue = dht.readTemperature();

  // Aggiornamento del buffer e calcolo della media
  int pos = counterValues % maxValues;
  sumH = sumH - humValues[pos] + humValue;
  humValues[pos] = humValue;
  
  sumT = sumT - temperValues[pos] + temValue;
  temperValues[pos] = temValue;
  
  counterValues++;

  if (counterValues < maxValues) {
    avgT = sumT / counterValues;
    avgH = sumH / counterValues;
  } else {
    avgT = sumT / maxValues;
    avgH = sumH / maxValues;
  }
}

float SensorModule::getTemperature() {
  return avgT;
}

float SensorModule::getHumidity() {
  return avgH;
}
