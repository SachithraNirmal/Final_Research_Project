#include <Arduino.h>
#include "WiFiManager.h"

void WiFiManager::connect() {
    Serial.println("Starting WiFi Connection Procedure");
    Serial.println("Target Network: " + String(WIFI_SSID));
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    connectionStartTime = millis();
    Serial.print("Establishing WiFi Connection");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
        
        if (millis() - connectionStartTime > NETWORK_TIMEOUT) {
            Serial.println("\nWiFi Connection Failed: Timeout exceeded");
            Serial.println("Initiating System Restart...");
            ESP.restart();
        }
    }
    
    Serial.println("\nWiFi Connection Established Successfully");
    Serial.println("Local IP Address: " + WiFi.localIP().toString());
    Serial.println("Signal Strength: " + String(WiFi.RSSI()) + " dBm");
}

void WiFiManager::printStatus() {
    Serial.println("Network Status:");
    Serial.println("  WiFi Connected: " + String(isConnected() ? "Yes" : "No"));
    Serial.println("  IP Address: " + WiFi.localIP().toString());
    Serial.println("  Signal Strength: " + String(WiFi.RSSI()) + " dBm");
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}