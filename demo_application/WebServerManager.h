#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "DeviceManager.h"
#include "BACnet_ESP8266.h"

class WebServerManager {
private:
    WiFiServer server;
    DeviceManager* deviceManager;
    BACnet_ESP8266* bacnetController;
    
public:
    WebServerManager(DeviceManager* dm, BACnet_ESP8266* bacnet) 
        : server(WEB_SERVER_PORT), deviceManager(dm), bacnetController(bacnet) {}
    
    void begin();
    void handleClient();
    
private:
    void sendMainPage(WiFiClient& client);
    void sendJSONStatus(WiFiClient& client);
    void handleLightControl(WiFiClient& client, const String& body);
    void handleACControl(WiFiClient& client, const String& body);
    void handleTemperatureControl(WiFiClient& client, const String& body);
    String getHTMLContent();
};

#endif