#include <Arduino.h>
#include "SensorManager.h"

void SensorManager::begin() {
    Serial.println("Starting DHT11 temperature and humidity sensor...");
    dht.begin();
}

void SensorManager::readAndUploadData() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastDHTUpload >= DHT_UPLOAD_INTERVAL) {
        lastDHTUpload = currentTime;
        readDHTSensor();
    }
}

void SensorManager::readDHTSensor() {
    Serial.println("Reading DHT11 temperature and humidity sensor...");
    
    float tempReading = dht.readTemperature();
    float humidityReading = dht.readHumidity();
    
    if (!isnan(tempReading) && !isnan(humidityReading)) {
        temperature = tempReading;
        humidity = humidityReading;
        Serial.println("DHT Sensor Read Successful:");
        Serial.println("  Temperature: " + String(temperature) + " C");
        Serial.println("  Humidity: " + String(humidity) + " %");
    } else {
        Serial.println("DHT Sensor Error: Failed to read temperature or humidity");
        Serial.println("  Temperature Read: " + String(isnan(tempReading) ? "Failed" : "Success"));
        Serial.println("  Humidity Read: " + String(isnan(humidityReading) ? "Failed" : "Success"));
    }
}

void SensorManager::printStatus() {
    Serial.println("Environmental Sensor Status:");
    Serial.println("  Temperature: " + String(isnan(temperature) ? "Reading Failed" : String(temperature) + " C"));
    Serial.println("  Humidity: " + String(isnan(humidity) ? "Reading Failed" : String(humidity) + " %"));
}

float SensorManager::getTemperature() { return temperature; }
float SensorManager::getHumidity() { return humidity; }