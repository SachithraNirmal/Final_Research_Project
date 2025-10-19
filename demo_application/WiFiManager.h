#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "Config.h"

class WiFiManager {
public:
    bool connect() {
        Serial.println("\n Connecting to WiFi: " + String(WIFI_SSID));
        
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(1000);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n WiFi connected!");
            Serial.println(" IP address: " + WiFi.localIP().toString());
            return true;
        } else {
            Serial.println("\n WiFi connection failed!");
            return false;
        }
    }
    
    String getIPAddress() {
        return WiFi.localIP().toString();
    }
    
    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif