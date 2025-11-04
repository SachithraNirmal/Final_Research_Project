#include <Arduino.h>
#include "BACnetProtocol.h"

void BACnetProtocol::begin() {
    Serial.println("Initializing BACnet Protocol Stack");
    
    if (bacnetUDP.begin(BACNET_PORT)) {
        Serial.println("BACnet UDP Service Started Successfully");
        Serial.println("Listening Port: " + String(BACNET_PORT));
        Serial.println("Device Configuration:");
        Serial.println("  Device ID: " + String(DEVICE_ID));
        Serial.println("  Device Name: " + String(deviceObject.object_name));
        Serial.println("  Vendor ID: " + String(VENDOR_ID));
        Serial.println("  Vendor Name: Sachithra");
        Serial.println("  Maximum APDU: " + String(MAX_APDU) + " bytes");
    } else {
        Serial.println("BACnet UDP Service Failed to Start");
        Serial.println("Critical Error: BACnet functionality will not be available");
    }
}

void BACnetProtocol::handle() {
    int packetSize = bacnetUDP.parsePacket();
    if (packetSize) {
        uint8_t packetBuffer[512];
        int packetLength = bacnetUDP.read(packetBuffer, sizeof(packetBuffer));
        IPAddress remoteAddress = bacnetUDP.remoteIP();
        uint16_t remotePort = bacnetUDP.remotePort();
        
        Serial.println("BACnet Packet Received");
        Serial.println("  Source: " + remoteAddress.toString() + ":" + String(remotePort));
        Serial.println("  Packet Size: " + String(packetLength) + " bytes");
        
        Serial.print("  Packet Data (Hex): ");
        for(int i = 0; i < (packetLength < 16 ? packetLength : 16); i++) {
            Serial.printf("%02X ", packetBuffer[i]);
        }
        Serial.println();
        
        processBACnetPacket(packetBuffer, packetLength, remoteAddress, remotePort);
    }
}

void BACnetProtocol::broadcastPresence() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastBACnetDiscovery >= BACNET_DISCOVERY_INTERVAL) {
        lastBACnetDiscovery = currentTime;
        sendIAm();
    }
}

void BACnetProtocol::printStatus() {
    Serial.println("BACnet Protocol Status:");
    Serial.println("  Service: Running on Port " + String(BACNET_PORT));
    Serial.println("  Device ID: " + String(DEVICE_ID));
    Serial.println("  Device Name: " + String(deviceObject.object_name));
    Serial.println("  Objects Available: 6 (Device, 2 Outputs, 2 Inputs, 1 Binary Input)");
}

void BACnetProtocol::processBACnetPacket(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort) {
    if (len < 4) {
        Serial.println("BACnet Error: Packet too short to process");
        return;
    }
    
    // BACnet/IP header
    if (buffer[0] != 0x81) {
        Serial.println("BACnet Error: Invalid packet header, not a BACnet/IP packet");
        return;
    }
    
    uint8_t pduType = (buffer[1] >> 4) & 0x0F;
    Serial.println("BACnet PDU Type Identified: " + String(pduType));
    
    //Hhandler based on PDU type
    switch (pduType) {
        case 0: // Unconfirmed request
            Serial.println("Processing Unconfirmed Request");
            handleUnconfirmedRequest(buffer, len, remoteIP, remotePort);
            break;
        case 1: // Confirmed request
            Serial.println("Processing Confirmed Request");
            handleConfirmedRequest(buffer, len, remoteIP, remotePort);
            break;
        default:
            Serial.println("BACnet Warning: Unsupported PDU type received: " + String(pduType));
            break;
    }
}

void BACnetProtocol::handleUnconfirmedRequest(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort) {
    if (len < 5) {
        Serial.println("BACnet Error: Unconfirmed request packet too short");
        return;
    }
    
    uint8_t serviceChoice = buffer[4];
    Serial.println("Unconfirmed Service Request: " + String(serviceChoice));
    
    switch (serviceChoice) {
        case 0x08: // Who-Is service
            Serial.println("BACnet Who-Is Request Received");
            Serial.println("Responding with I-Am broadcast");
            sendIAm();
            break;
        case 0x00: // I-Am service
            Serial.println("BACnet I-Am Received from another device (ignored)");
            break;
        default:
            Serial.println("BACnet Warning: Unsupported unconfirmed service: " + String(serviceChoice));
            break;
    }
}

void BACnetProtocol::handleConfirmedRequest(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort) {
    if (len < 7) {
        Serial.println("BACnet Error: Confirmed request packet too short");
        return;
    }
    
    uint8_t invokeId = buffer[5];
    uint8_t serviceChoice = buffer[6];
    
    Serial.println("Confirmed Service Request Details:");
    Serial.println("  Invocation ID: " + String(invokeId));
    Serial.println("  Service Type: " + String(serviceChoice));
    
    switch (serviceChoice) {
        case 0x0C: // ReadProperty
            Serial.println("Processing ReadProperty Request");
            handleReadProperty(buffer, len, remoteIP, remotePort, invokeId);
            break;
        case 0x0F: // WriteProperty
            Serial.println("Processing WriteProperty Request");
            handleWriteProperty(buffer, len, remoteIP, remotePort, invokeId);
            break;
        default:
            Serial.println("BACnet Error: Unsupported confirmed service");
            sendError(remoteIP, remotePort, invokeId, 0, 0); // Service not supported
            break;
    }
}

void BACnetProtocol::handleReadProperty(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId) {
    if (len < 10) {
        Serial.println("BACnet Error: ReadProperty request packet too short");
        sendError(remoteIP, remotePort, invokeId, 0, 0);
        return;
    }
    
    // Decode object identifier
    uint16_t requestedObjectType;
    uint32_t requestedObjectInstance = decodeBACnetObjectId(&buffer[7], &requestedObjectType);
    
    // Locate property identifier
    uint8_t propertyTagPosition = 11;
    if (propertyTagPosition >= len) {
        Serial.println("BACnet Error: Property identifier not found in packet");
        sendError(remoteIP, remotePort, invokeId, 0, 0);
        return;
    }
    
    uint32_t requestedPropertyId = decodeBACnetUnsigned(&buffer[propertyTagPosition], len - propertyTagPosition);
    
    Serial.println("ReadProperty Request Details:");
    Serial.println("  Object Type: " + String(requestedObjectType));
    Serial.println("  Object Instance: " + String(requestedObjectInstance));
    Serial.println("  Property ID: " + String(requestedPropertyId));
    
    sendReadPropertyACK(remoteIP, remotePort, invokeId, requestedObjectType, requestedObjectInstance, requestedPropertyId);
}

void BACnetProtocol::handleWriteProperty(uint8_t* buffer, size_t len, IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId) {
    Serial.println("WriteProperty Request Received - Feature Not Fully Implemented");
    Serial.println("Sending Error Response: Service Not Implemented");
    sendError(remoteIP, remotePort, invokeId, 0, 0);
}

void BACnetProtocol::sendIAm() {
    Serial.println("Preparing BACnet I-Am Broadcast Message");
    
    uint8_t broadcastBuffer[50];
    int bufferPosition = 0;
    
    // BACnet/IP BVLC Header
    broadcastBuffer[bufferPosition++] = 0x81; 
    broadcastBuffer[bufferPosition++] = 0x00; 
    broadcastBuffer[bufferPosition++] = 0x00; 
    broadcastBuffer[bufferPosition++] = 0x16; 
    
    // NPDU (Network Protocol Data Unit)
    broadcastBuffer[bufferPosition++] = 0x01; 
    broadcastBuffer[bufferPosition++] = 0x00;
    broadcastBuffer[bufferPosition++] = 0x00; 
    broadcastBuffer[bufferPosition++] = 0xFF; 
    broadcastBuffer[bufferPosition++] = 0xFF;
    
    // APDU (Application Protocol Data Unit)
    broadcastBuffer[bufferPosition++] = 0x10; // I-Am service send Message
    
    // device object identifier
    encodeBACnetObjectId(&broadcastBuffer[bufferPosition], OBJECT_DEVICE, DEVICE_ID);
    bufferPosition += 4;
    
    // maximum APDU size
    encodeBACnetUnsigned(&broadcastBuffer[bufferPosition], MAX_APDU);
    bufferPosition += 3;
    
    broadcastBuffer[bufferPosition++] = 0x00; 
    
    // Vendor identifier
    encodeBACnetUnsigned(&broadcastBuffer[bufferPosition], VENDOR_ID);
    bufferPosition += 2;
    
    // Send broadcast to entire network
    IPAddress broadcastAddress(255, 255, 255, 255);
    bacnetUDP.beginPacket(broadcastAddress, BACNET_PORT);
    bacnetUDP.write(broadcastBuffer, bufferPosition);
    bacnetUDP.endPacket();
    
    Serial.println("BACnet I-Am Broadcast Sent Successfully");
    Serial.println("  Device: " + String(deviceObject.object_name));
    Serial.println("  Device ID: " + String(DEVICE_ID));
    Serial.println("  Vendor: Sachithra");
    Serial.println("  Maximum APDU: " + String(MAX_APDU));
}

void BACnetProtocol::sendReadPropertyACK(IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId, 
                        uint16_t objectType, uint32_t objectInstance, uint32_t propertyId) {
    Serial.println("Preparing ReadProperty Acknowledgement Response");
    
    uint8_t responseBuffer[128];
    int bufferPosition = 0;
    
    // BVLC Header
    responseBuffer[bufferPosition++] = 0x81; 
    responseBuffer[bufferPosition++] = 0x0a;
    responseBuffer[bufferPosition++] = 0x00;
    responseBuffer[bufferPosition++] = 0x00;
    
    // NPDU
    responseBuffer[bufferPosition++] = 0x01; 
    responseBuffer[bufferPosition++] = 0x00; 
    
    // APDU
    responseBuffer[bufferPosition++] = 0x04;
    responseBuffer[bufferPosition++] = invokeId;
    responseBuffer[bufferPosition++] = 0x0c;
    
    // Object identifier
    encodeBACnetObjectId(&responseBuffer[bufferPosition], objectType, objectInstance);
    bufferPosition += 4;
    
    // Property identifier
    encodeBACnetUnsigned(&responseBuffer[bufferPosition], propertyId);
    bufferPosition += (propertyId <= 255) ? 2 : 3;
    
    // Requested property value
    bool propertyAvailable = true;
    
    Serial.println("Processing Property Request: " + String(propertyId));
    
    switch (propertyId) {
        case PROP_OBJECT_IDENTIFIER:
            Serial.println("Property: Object Identifier");
            encodeBACnetObjectId(&responseBuffer[bufferPosition], objectType, objectInstance);
            bufferPosition += 4;
            break;
            
        case PROP_OBJECT_NAME:
            Serial.println("Property: Object Name");
            if (objectType == OBJECT_DEVICE && objectInstance == DEVICE_ID) {
                encodeBACnetCharacterString(&responseBuffer[bufferPosition], deviceObject.object_name);
                bufferPosition += strlen(deviceObject.object_name) + 2;
                Serial.println("  Value: " + String(deviceObject.object_name));
            } else if (objectType == OBJECT_BINARY_OUTPUT && objectInstance == 1) {
                encodeBACnetCharacterString(&responseBuffer[bufferPosition], binaryOutput1.object_name);
                bufferPosition += strlen(binaryOutput1.object_name) + 2;
                Serial.println("  Value: " + String(binaryOutput1.object_name));
            } else if (objectType == OBJECT_ANALOG_OUTPUT && objectInstance == 2) {
                encodeBACnetCharacterString(&responseBuffer[bufferPosition], analogOutput1.object_name);
                bufferPosition += strlen(analogOutput1.object_name) + 2;
                Serial.println("  Value: " + String(analogOutput1.object_name));
            } else if (objectType == OBJECT_ANALOG_INPUT && objectInstance == 3) {
                encodeBACnetCharacterString(&responseBuffer[bufferPosition], analogInput1.object_name);
                bufferPosition += strlen(analogInput1.object_name) + 2;
                Serial.println("  Value: " + String(analogInput1.object_name));
            } else if (objectType == OBJECT_ANALOG_INPUT && objectInstance == 4) {
                encodeBACnetCharacterString(&responseBuffer[bufferPosition], analogInput2.object_name);
                bufferPosition += strlen(analogInput2.object_name) + 2;
                Serial.println("  Value: " + String(analogInput2.object_name));
            }
            break;
            
        case PROP_OBJECT_TYPE:
            Serial.println("Property: Object Type");
            encodeBACnetUnsigned(&responseBuffer[bufferPosition], objectType);
            bufferPosition += (objectType <= 255) ? 2 : 3;
            Serial.println("  Value: " + String(objectType));
            break;
            
        case PROP_PRESENT_VALUE:
            Serial.println("Property: Present Value");
            if (objectType == OBJECT_BINARY_OUTPUT && objectInstance == 1) {
                encodeBACnetReal(&responseBuffer[bufferPosition], binaryOutput1.present_value);
                bufferPosition += 4;
                Serial.println("  Value: " + String(binaryOutput1.present_value));
            } else if (objectType == OBJECT_ANALOG_OUTPUT && objectInstance == 2) {
                encodeBACnetReal(&responseBuffer[bufferPosition], analogOutput1.present_value);
                bufferPosition += 4;
                Serial.println("  Value: " + String(analogOutput1.present_value));
            } else if (objectType == OBJECT_ANALOG_INPUT && objectInstance == 3) {
                encodeBACnetReal(&responseBuffer[bufferPosition], analogInput1.present_value);
                bufferPosition += 4;
                Serial.println("  Value: " + String(analogInput1.present_value));
            } else if (objectType == OBJECT_ANALOG_INPUT && objectInstance == 4) {
                encodeBACnetReal(&responseBuffer[bufferPosition], analogInput2.present_value);
                bufferPosition += 4;
                Serial.println("  Value: " + String(analogInput2.present_value));
            }
            break;
            
        case PROP_SYSTEM_STATUS:
            Serial.println("Property: System Status");
            encodeBACnetUnsigned(&responseBuffer[bufferPosition], 0);
            bufferPosition += 2;
            Serial.println("  Value: Operational (0)");
            break;
            
        case PROP_VENDOR_NAME:
            Serial.println("Property: Vendor Name");
            encodeBACnetCharacterString(&responseBuffer[bufferPosition], "Sachithra");
            bufferPosition += strlen("Sachithra") + 2;
            Serial.println("  Value: Sachithra");
            break;
            
        case PROP_VENDOR_IDENTIFIER:
            Serial.println("Property: Vendor Identifier");
            encodeBACnetUnsigned(&responseBuffer[bufferPosition], VENDOR_ID);
            bufferPosition += 2;
            Serial.println("  Value: " + String(VENDOR_ID));
            break;
            
        default:
            Serial.println("Property Error: Unknown property requested - " + String(propertyId));
            propertyAvailable = false;
            sendError(remoteIP, remotePort, invokeId, 0, 0); // Unknown property error
            return;
    }
    
    if (propertyAvailable) {
        // Update packet length in BVLC header
        uint16_t totalPacketLength = bufferPosition;
        responseBuffer[2] = (totalPacketLength >> 8) & 0xFF;
        responseBuffer[3] = totalPacketLength & 0xFF;
        
        // Send response to requesting device
        bacnetUDP.beginPacket(remoteIP, remotePort);
        bacnetUDP.write(responseBuffer, bufferPosition);
        bacnetUDP.endPacket();
        
        Serial.println("ReadProperty Acknowledgement Sent Successfully");
        Serial.println("  Destination: " + remoteIP.toString() + ":" + String(remotePort));
        Serial.println("  Total Packet Size: " + String(totalPacketLength) + " bytes");
    }
}

void BACnetProtocol::sendError(IPAddress remoteIP, uint16_t remotePort, uint8_t invokeId, uint8_t errorClass, uint8_t errorCode) {
    Serial.println("Preparing BACnet Error Response");
    
    uint8_t errorBuffer[32];
    int bufferPosition = 0;
    
    // BVLC Header
    errorBuffer[bufferPosition++] = 0x81;
    errorBuffer[bufferPosition++] = 0x0a;
    errorBuffer[bufferPosition++] = 0x00;
    errorBuffer[bufferPosition++] = 0x0c; // Fixed length of 12 bytes
    
    // NPDU
    errorBuffer[bufferPosition++] = 0x01;
    errorBuffer[bufferPosition++] = 0x00;
    
    // APDU - Error PDU
    errorBuffer[bufferPosition++] = 0x05; // Error PDU type
    errorBuffer[bufferPosition++] = invokeId;
    errorBuffer[bufferPosition++] = 0x0e; // Error service
    
    // Error parameters
    errorBuffer[bufferPosition++] = errorClass;
    errorBuffer[bufferPosition++] = errorCode;
    
    bacnetUDP.beginPacket(remoteIP, remotePort);
    bacnetUDP.write(errorBuffer, bufferPosition);
    bacnetUDP.endPacket();
    
    Serial.println("BACnet Error Response Sent");
    Serial.println("  Error Class: " + String(errorClass));
    Serial.println("  Error Code: " + String(errorCode));
    Serial.println("  Destination: " + remoteIP.toString() + ":" + String(remotePort));
}

// BACnet Encoding/Decoding Functions
void BACnetProtocol::encodeBACnetObjectId(uint8_t* buffer, uint16_t objectType, uint32_t objectInstance) {
    buffer[0] = (objectType >> 8) & 0xFF;
    buffer[1] = objectType & 0xFF;
    buffer[2] = (objectInstance >> 8) & 0xFF;
    buffer[3] = objectInstance & 0xFF;
}

void BACnetProtocol::encodeBACnetUnsigned(uint8_t* buffer, uint32_t value) {
    if (value <= 255) {
        buffer[0] = 0x21; // Context tag, length 1
        buffer[1] = value;
    } else if (value <= 65535) {
        buffer[0] = 0x22; // Context tag, length 2
        buffer[1] = (value >> 8) & 0xFF;
        buffer[2] = value & 0xFF;
    } else {
        buffer[0] = 0x24; // Context tag, length 4
        buffer[1] = (value >> 24) & 0xFF;
        buffer[2] = (value >> 16) & 0xFF;
        buffer[3] = (value >> 8) & 0xFF;
        buffer[4] = value & 0xFF;
    }
}

void BACnetProtocol::encodeBACnetReal(uint8_t* buffer, float value) {
    buffer[0] = 0x44; // Application tag, real (4 bytes)
    
    // Convert float to IEEE 754 representation
    union {
        float floatValue;
        uint32_t intValue;
    } floatConverter;
    
    floatConverter.floatValue = value;
    buffer[1] = (floatConverter.intValue >> 24) & 0xFF;
    buffer[2] = (floatConverter.intValue >> 16) & 0xFF;
    buffer[3] = (floatConverter.intValue >> 8) & 0xFF;
    buffer[4] = floatConverter.intValue & 0xFF;
}

void BACnetProtocol::encodeBACnetCharacterString(uint8_t* buffer, const char* str) {
    buffer[0] = 0x75; // Application tag, character string
    buffer[1] = strlen(str); // String length
    memcpy(&buffer[2], str, strlen(str)); // String content
}

uint16_t BACnetProtocol::decodeBACnetUnsigned(uint8_t* buffer, uint8_t len) {
    if (len < 2) {
        Serial.println("BACnet Decode Error: Buffer too short for unsigned integer");
        return 0;
    }
    
    uint8_t tagByte = buffer[0];
    uint8_t valueLength = tagByte & 0x07;
    
    if (valueLength == 1) {
        return buffer[1];
    } else if (valueLength == 2) {
        return (buffer[1] << 8) | buffer[2];
    }
    
    Serial.println("BACnet Decode Warning: Unsupported unsigned integer length");
    return 0;
}

uint32_t BACnetProtocol::decodeBACnetObjectId(uint8_t* buffer, uint16_t* objectType) {
    *objectType = (buffer[0] << 8) | buffer[1];
    return (buffer[2] << 8) | buffer[3];
}

void BACnetProtocol::updateBinaryOutput(uint32_t instance, float value) {
    if (instance == 1) {
        binaryOutput1.present_value = value;
    }
}

void BACnetProtocol::updateAnalogOutput(uint32_t instance, float value) {
    if (instance == 2) {
        analogOutput1.present_value = value;
    }
}

void BACnetProtocol::updateAnalogInput(uint32_t instance, float value) {
    if (instance == 3) {
        analogInput1.present_value = value;
    } else if (instance == 4) {
        analogInput2.present_value = value;
    }
}