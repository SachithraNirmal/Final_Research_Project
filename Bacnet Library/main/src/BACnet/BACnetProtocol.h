#ifndef BACNET_PROTOCOL_H
#define BACNET_PROTOCOL_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include "../config/config.h"

// BACnet Constants
#define OBJECT_ANALOG_INPUT 0
#define OBJECT_ANALOG_OUTPUT 1
#define OBJECT_BINARY_INPUT 3
#define OBJECT_BINARY_OUTPUT 4
#define OBJECT_DEVICE 8

// BACnet Property Identifiers
#define PROP_OBJECT_IDENTIFIER 75
#define PROP_OBJECT_NAME 77
#define PROP_OBJECT_TYPE 79
#define PROP_SYSTEM_STATUS 112
#define PROP_VENDOR_NAME 99
#define PROP_VENDOR_IDENTIFIER 96
#define PROP_PRESENT_VALUE 85

// BACnet Object Structure Definition
typedef struct {
    uint32_t object_id;
    uint16_t object_type;
    char object_name[32];
    float present_value;
    char description[64];
} BACnetObject;

class BACnetProtocol {
public:
    void begin();
    void handle();
    void broadcastPresence();
    void printStatus();
    
    // Callbacks for device state updates
    void updateBinaryOutput(uint32_t instance, float value);
    void updateAnalogOutput(uint32_t instance, float value);
    void updateAnalogInput(uint32_t instance, float value);

private:
    WiFiUDP bacnetUDP;
    uint32_t bacnetInvokeId = 1;
    unsigned long lastBACnetDiscovery = 0;
    
    // BACnet Objects
    BACnetObject deviceObject = {DEVICE_ID, OBJECT_DEVICE, "SBMCon", 0.0, "Smart Building Controller"};
    BACnetObject binaryOutput1 = {1, OBJECT_BINARY_OUTPUT, "Digital_LED", 0.0, "Digital LED Output"};
    BACnetObject analogOutput1 = {2, OBJECT_ANALOG_OUTPUT, "Dimming_LED", 0.0, "Dimming LED Output"};
    BACnetObject analogInput1 = {3, OBJECT_ANALOG_INPUT, "Temperature", 0.0, "Temperature Sensor"};
    BACnetObject analogInput2 = {4, OBJECT_ANALOG_INPUT, "Humidity", 0.0, "Humidity Sensor"};
    BACnetObject binaryInput1 = {5, OBJECT_BINARY_INPUT, "Button_State", 0.0, "Manual Button Input"};
    
    void processBACnetPacket(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort);
    void handleUnconfirmedRequest(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort);
    void handleConfirmedRequest(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort);
    void handleReadProperty(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId);
    void handleWriteProperty(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId);
    void sendIAm();
    void sendReadPropertyACK(IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId, 
                            uint16_t objectType, uint32_t objectInstance, uint32_t propertyId);
    void sendError(IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId, uint8_t errorClass, uint8_t errorCode);
    
    // Encoding/Decoding functions
    void encodeBACnetObjectId(uint8_t* buffer, uint16_t objectType, uint32_t objectInstance);
    void encodeBACnetUnsigned(uint8_t* buffer, uint32_t value);
    void encodeBACnetReal(uint8_t* buffer, float value);
    void encodeBACnetCharacterString(uint8_t* buffer, const char* str);
    uint16_t decodeBACnetUnsigned(uint8_t* buffer, uint8_t len);
    uint32_t decodeBACnetObjectId(uint8_t* buffer, uint16_t* objectType);
};

#endif