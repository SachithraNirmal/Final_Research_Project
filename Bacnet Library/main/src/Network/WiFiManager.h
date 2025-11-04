#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../config/credentials.h"
#include "../config/config.h"

class WiFiManager {
public:
    void connect();
    void printStatus();
    bool isConnected();

private:
    unsigned long connectionStartTime = 0;
};

#endif