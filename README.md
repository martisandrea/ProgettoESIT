# ProgettoESIT

Progetto per la gestione di sensori, invio di dati tramite MQTT e visualizzazione su un display LCD. Il progetto utilizza una combinazione di moduli hardware e librerie software per monitorare temperatura e umidità, inviare dati a un broker MQTT e visualizzare informazioni su un display LCD.

## Struttura del Progetto

La struttura del progetto è organizzata come segue:


## Funzionalità Principali

- **Connessione Wi-Fi**: Gestisce la connessione a una rete Wi-Fi.
- **Sincronizzazione dell'ora**: Utilizza il protocollo SNTP per sincronizzare l'orologio del dispositivo.
- **Gestione MQTT**: Pubblica dati dei sensori e invia avvisi critici a un broker MQTT.
- **Visualizzazione su LCD**: Mostra messaggi e dati sul display LCD tramite I2C.
- **Monitoraggio dei sensori**: Legge i dati di temperatura e umidità da un modulo sensore.

## Librerie Utilizzate

Il progetto utilizza le seguenti librerie:

- [ArduinoJson](https://arduinojson.org/): Per la gestione dei dati JSON.
- [MQTT](https://github.com/256dpi/arduino-mqtt): Per la comunicazione MQTT.
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C): Per la gestione del display LCD.
- [NTPClient](https://github.com/arduino-libraries/NTPClient): Per la sincronizzazione dell'ora.
- [DHT_sensor_library](https://github.com/adafruit/DHT-sensor-library): Per la lettura dei dati di temperatura e umidità.

## Hardware Necessario

- Modulo Wi-Fi (es. ESP8266 o ESP32)
- Sensore di temperatura e umidità (es. DHT11 o DHT22)
- Display LCD con interfaccia I2C
- Alimentazione adeguata per i componenti

## Configurazione

1. Modifica il file `configuration.h` per impostare i parametri di rete e MQTT:
   ```cpp
   #define ssid "NomeReteWiFi"
   #define password "PasswordWiFi"
   #define THINGNAME "NomeDispositivo"
   #define MQTT_HOST "IndirizzoBrokerMQTT"