#ifndef BACNET_ESP8266_H
#define BACNET_ESP8266_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Config.h"

// BACnet Object Types
#define BACNET_OBJECT_ANALOG_INPUT 0
#define BACNET_OBJECT_ANALOG_OUTPUT 1
#define BACNET_OBJECT_BINARY_INPUT 3
#define BACNET_OBJECT_BINARY_OUTPUT 4

// BACnet Property Identifiers
#define BACNET_PROP_OBJECT_ID 75
#define BACNET_PROP_OBJECT_NAME 77
#define BACNET_PROP_PRESENT_VALUE 85

// BACnet Services
#define BACNET_SERVICE_READ_PROPERTY 12
#define BACNET_SERVICE_WRITE_PROPERTY 15

class BACnet_ESP8266 {
private:
    WiFiUDP udp;
    IPAddress localIP;
    uint16_t localPort = 47808; // BACnet standard port
    
    uint32_t deviceInstance = 12345; // Default device instance
    uint8_t buffer[512];
    
    typedef struct {
        uint32_t objectId;
        uint8_t objectType;
        char objectName[32];
        float presentValue;
    } BACnetObject;
    
    BACnetObject* objects;
    uint8_t objectCount;
    
    // BACnet PDU functions
    void encodeReadProperty(uint8_t* buffer, uint16_t* length, 
                           uint32_t deviceId, uint32_t objectId, 
                           uint8_t objectType, uint32_t propertyId);
    
    void encodeWriteProperty(uint8_t* buffer, uint16_t* length,
                           uint32_t deviceId, uint32_t objectId,
                           uint8_t objectType, uint32_t propertyId,
                           float value);
    
    bool decodeAPDU(uint8_t* apdu, uint16_t apduLen, 
                   uint32_t* invokingDevice, uint8_t* serviceChoice,
                   uint32_t* objectId, uint8_t* objectType,
                   uint32_t* propertyId, float* value);
    
    void sendErrorResponse(uint8_t* buffer, uint16_t* length, uint8_t errorCode);

public:
    BACnet_ESP8266();
    ~BACnet_ESP8266();
    
    bool begin(uint32_t deviceInstance = 12345);
    void update();
    
    // Object management
    bool addObject(uint8_t objectType, uint32_t objectId, const char* objectName, float initialValue = 0.0);
    bool setPresentValue(uint32_t objectId, float value);
    float getPresentValue(uint32_t objectId);
    
    // Object management for building automation
    void updateBuildingObjects(bool lightState, float lightBrightness, float temperature, 
                              bool acState, const String& acMode, int fanSpeed) {
        setPresentValue(LIGHT_OBJECT_ID, lightState ? 1.0 : 0.0);
        setPresentValue(LIGHT_BRIGHTNESS_OBJECT_ID, lightBrightness);
        setPresentValue(TEMPERATURE_OBJECT_ID, temperature);
        setPresentValue(AC_STATE_OBJECT_ID, acState ? 1.0 : 0.0);
        
        // Convert AC mode to numeric value
        float modeValue = 1.0; // cool
        if (acMode == "heat") modeValue = 2.0;
        else if (acMode == "auto") modeValue = 3.0;
        setPresentValue(AC_MODE_OBJECT_ID, modeValue);
        
        setPresentValue(FAN_SPEED_OBJECT_ID, (float)fanSpeed);
    }
    
    // Network configuration
    void setDeviceInstance(uint32_t instance) { deviceInstance = instance; }
    uint32_t getDeviceInstance() { return deviceInstance; }
};

#endif