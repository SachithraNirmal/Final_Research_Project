#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "../config/pins.h"
#include "../config/config.h"

class SensorManager {
public:
    void begin();
    void readAndUploadData();
    void printStatus();
    float getTemperature();
    float getHumidity();

private:
    DHT dht = DHT(DHT11_AI, DHT11);
    float temperature = NAN;
    float humidity = NAN;
    unsigned long lastDHTUpload = 0;
    
    void readDHTSensor();
};

#endif