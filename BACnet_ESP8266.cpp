#include "BACnet_ESP8266.h"

BACnet_ESP8266::BACnet_ESP8266() {
    objects = nullptr;
    objectCount = 0;
}

BACnet_ESP8266::~BACnet_ESP8266() {
    if (objects != nullptr) {
        delete[] objects;
    }
}

bool BACnet_ESP8266::begin(uint32_t deviceInstance) {
    this->deviceInstance = deviceInstance;
    
    if (udp.begin(localPort)) {
        Serial.println("BACnet: UDP server started on port " + String(localPort));
        return true;
    }
    return false;
}

bool BACnet_ESP8266::addObject(uint8_t objectType, uint32_t objectId, const char* objectName, float initialValue) {
    // Resize objects array
    BACnetObject* newObjects = new BACnetObject[objectCount + 1];
    if (objects != nullptr) {
        memcpy(newObjects, objects, sizeof(BACnetObject) * objectCount);
        delete[] objects;
    }
    objects = newObjects;
    
    // Initialize new object
    objects[objectCount].objectId = objectId;
    objects[objectCount].objectType = objectType;
    strncpy(objects[objectCount].objectName, objectName, 31);
    objects[objectCount].presentValue = initialValue;
    
    objectCount++;
    Serial.println("BACnet: Added object " + String(objectName) + " (ID: " + String(objectId) + ")");
    return true;
}

bool BACnet_ESP8266::setPresentValue(uint32_t objectId, float value) {
    for (uint8_t i = 0; i < objectCount; i++) {
        if (objects[i].objectId == objectId) {
            objects[i].presentValue = value;
            Serial.println("BACnet: Object " + String(objectId) + " value set to " + String(value));
            return true;
        }
    }
    return false;
}

float BACnet_ESP8266::getPresentValue(uint32_t objectId) {
    for (uint8_t i = 0; i < objectCount; i++) {
        if (objects[i].objectId == objectId) {
            return objects[i].presentValue;
        }
    }
    return 0.0;
}

void BACnet_ESP8266::update() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        IPAddress remoteIP = udp.remoteIP();
        uint16_t remotePort = udp.remotePort();
        
        int len = udp.read(buffer, sizeof(buffer));
        
        uint32_t invokingDevice;
        uint8_t serviceChoice;
        uint32_t objectId;
        uint8_t objectType;
        uint32_t propertyId;
        float value;
        
        if (decodeAPDU(buffer, len, &invokingDevice, &serviceChoice, &objectId, &objectType, &propertyId, &value)) {
            switch (serviceChoice) {
                case BACNET_SERVICE_READ_PROPERTY:
                    Serial.println("BACnet: ReadProperty request for object " + String(objectId));
                    // Handle read property - simplified implementation
                    for (uint8_t i = 0; i < objectCount; i++) {
                        if (objects[i].objectId == objectId) {
                            // Send response with present value
                            uint16_t responseLength = 0;
                            encodeReadProperty(buffer, &responseLength, deviceInstance, objectId, objectType, propertyId);
                            udp.beginPacket(remoteIP, remotePort);
                            udp.write(buffer, responseLength);
                            udp.endPacket();
                            break;
                        }
                    }
                    break;
                    
                case BACNET_SERVICE_WRITE_PROPERTY:
                    Serial.println("BACnet: WriteProperty request for object " + String(objectId) + " value: " + String(value));
                    if (setPresentValue(objectId, value)) {
                        // Send success response
                        uint16_t responseLength = 0;
                        encodeWriteProperty(buffer, &responseLength, deviceInstance, objectId, objectType, propertyId, value);
                        udp.beginPacket(remoteIP, remotePort);
                        udp.write(buffer, responseLength);
                        udp.endPacket();
                    }
                    break;
            }
        }
    }
}

// Simplified BACnet PDU encoding/decoding (minimal implementation)
void BACnet_ESP8266::encodeReadProperty(uint8_t* buffer, uint16_t* length, 
                                       uint32_t deviceId, uint32_t objectId, 
                                       uint8_t objectType, uint32_t propertyId) {
    // Simplified BACnet ReadProperty response
    uint16_t idx = 0;
    
    // BVLC header (simplified)
    buffer[idx++] = 0x81; // BACnet/IP
    buffer[idx++] = 0x0A; // Original Unicast NPDU
    buffer[idx++] = 0x00; // Length high
    buffer[idx++] = 0x0C; // Length low
    
    // APDU
    buffer[idx++] = 0x00; // Version
    buffer[idx++] = 0x00; // Reserved
    
    *length = idx;
}

void BACnet_ESP8266::encodeWriteProperty(uint8_t* buffer, uint16_t* length,
                                       uint32_t deviceId, uint32_t objectId,
                                       uint8_t objectType, uint32_t propertyId,
                                       float value) {
    // Simplified BACnet WriteProperty response
    uint16_t idx = 0;
    
    // BVLC header (simplified)
    buffer[idx++] = 0x81; // BACnet/IP
    buffer[idx++] = 0x0A; // Original Unicast NPDU
    buffer[idx++] = 0x00; // Length high
    buffer[idx++] = 0x0C; // Length low
    
    // APDU
    buffer[idx++] = 0x00; // Version
    buffer[idx++] = 0x00; // Reserved
    
    *length = idx;
}

bool BACnet_ESP8266::decodeAPDU(uint8_t* apdu, uint16_t apduLen, 
                               uint32_t* invokingDevice, uint8_t* serviceChoice,
                               uint32_t* objectId, uint8_t* objectType,
                               uint32_t* propertyId, float* value) {
    // Simplified BACnet APDU decoding
    if (apduLen < 10) return false;
    
    // Skip BVLC header (simplified)
    uint16_t idx = 4;
    
    // Check for BACnet/IP
    if (apdu[0] != 0x81) return false;
    
    // Extract basic information (simplified)
    *invokingDevice = 12345; // Default
    *serviceChoice = apdu[idx++];
    *objectId = (apdu[idx] << 24) | (apdu[idx+1] << 16) | (apdu[idx+2] << 8) | apdu[idx+3];
    idx += 4;
    *objectType = apdu[idx++];
    *propertyId = apdu[idx++];
    
    // Extract value (simplified)
    if (*serviceChoice == BACNET_SERVICE_WRITE_PROPERTY && idx + 4 <= apduLen) {
        memcpy(value, &apdu[idx], 4);
    }
    
    return true;
}