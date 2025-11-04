#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <FirebaseESP8266.h>
#include <Arduino.h>
#include "../config/credentials.h"
#include "../config/config.h"

class FirebaseManager {
public:
    void begin();
    void syncInitialData();
    void syncData();
    void printStatus();
    bool isReady();
    
    void fetchBrightness();
    void fetchDigitalLed();
    void writeDigitalLed(bool state);
    void uploadSensorData(float temperature, float humidity);

private:
    FirebaseData fbdo;
    FirebaseConfig fbConfig;
    FirebaseAuth fbAuth;
    unsigned long lastFirebasePoll = 0;
};

#endif