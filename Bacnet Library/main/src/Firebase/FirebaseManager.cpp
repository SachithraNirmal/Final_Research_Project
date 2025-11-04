#include "FirebaseManager.h"
#include <Arduino.h>

void FirebaseManager::begin() {
    Serial.println("Initializing Firebase Cloud Service");
    
    fbConfig.database_url = FIREBASE_DB_URL;
    fbConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&fbConfig, &fbAuth);
    Firebase.reconnectWiFi(true);
    
    fbdo.setBSSLBufferSize(1024, 1024);
    fbdo.setResponseSize(1024);
    
    Serial.println("Firebase Service Initialized Successfully");
}

void FirebaseManager::syncInitialData() {
    Serial.println("Performing initial data synchronization...");
    fetchBrightness();
    fetchDigitalLed();
}

void FirebaseManager::syncData() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastFirebasePoll >= FIREBASE_POLL_INTERVAL) {
        lastFirebasePoll = currentTime;
        fetchBrightness();
        fetchDigitalLed();
    }
}

void FirebaseManager::fetchBrightness() {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for brightness fetch");
        return;
    }
    
    Serial.println("Fetching brightness level from Firebase...");
    
    if (Firebase.getInt(fbdo, PATH_BRIGHTNESS)) {
        int brightnessValue = fbdo.intData();
        Serial.println("Raw brightness value received: " + String(brightnessValue));
        
    } else {
        Serial.println("Firebase Error: Failed to read brightness value");
    }
}

void FirebaseManager::fetchDigitalLed() {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for LED state fetch");
        return;
    }

    Serial.println("Fetching digital LED state from Firebase...");

    if (Firebase.getBool(fbdo, PATH_DIGITAL_LED)) {
        bool remoteLedState = fbdo.boolData();
        Serial.println("Remote LED state received: " + String(remoteLedState ? "ON" : "OFF"));
        // This would typically call a callback to update the device
    } else {
        Serial.println("Firebase Error: Failed to read LED state");
    }
}

void FirebaseManager::writeDigitalLed(bool state) {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for LED state write");
        return;
    }

    Serial.println("Writing LED state to Firebase: " + String(state ? "ON" : "OFF"));

    if (Firebase.setBool(fbdo, PATH_DIGITAL_LED, state)) {
        Serial.println("LED state successfully synchronized with Firebase");
    } else {
        Serial.println("Firebase Error: Failed to write LED state");
    }
}

void FirebaseManager::uploadSensorData(float temperature, float humidity) {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for sensor data upload");
        return;
    }
    
    Serial.println("Uploading sensor data to Firebase...");
    
    FirebaseJson sensorData;
    sensorData.set("temperature", temperature);
    sensorData.set("humidity", humidity);

    if (Firebase.updateNode(fbdo, PATH_SENSOR, sensorData)) {
        Serial.println("Sensor data successfully uploaded to Firebase");
    } else {
        Serial.println("Firebase Error: Failed to upload sensor data");
    }
}

void FirebaseManager::printStatus() {
    Serial.println("Firebase Status:");
    Serial.println("  Connection: " + String(isReady() ? "Ready" : "Not Ready"));
}

bool FirebaseManager::isReady() {
    return Firebase.ready();
}#include "FirebaseManager.h"

void FirebaseManager::begin() {
    Serial.println("Initializing Firebase Cloud Service");
    
    fbConfig.database_url = FIREBASE_DB_URL;
    fbConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&fbConfig, &fbAuth);
    Firebase.reconnectWiFi(true);
    
    fbdo.setBSSLBufferSize(1024, 1024);
    fbdo.setResponseSize(1024);
    
    Serial.println("Firebase Service Initialized Successfully");
}

void FirebaseManager::syncData() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastFirebasePoll >= FIREBASE_POLL_INTERVAL) {
        lastFirebasePoll = currentTime;
        fetchBrightness();
        fetchDigitalLed();
    }
}

void FirebaseManager::fetchBrightness() {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for brightness fetch");
        return;
    }
    
    Serial.println("Fetching brightness level from Firebase...");
    
    if (Firebase.getInt(fbdo, PATH_BRIGHTNESS)) {
        int brightnessValue = fbdo.intData();
        Serial.println("Raw brightness value received: " + String(brightnessValue));
        
    } else {
        Serial.println("Firebase Error: Failed to read brightness value");
        Serial.println("Error Reason: " + String(fbdo.errorReason().c_str()));
    }
}

void FirebaseManager::fetchDigitalLed() {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for LED state fetch");
        return;
    }

    Serial.println("Fetching digital LED state from Firebase...");

    if (Firebase.getBool(fbdo, PATH_DIGITAL_LED)) {
        bool remoteLedState = fbdo.boolData();
        Serial.println("Remote LED state received: " + String(remoteLedState ? "ON" : "OFF"));
    } else {
        Serial.println("Firebase Error: Failed to read LED state");
        Serial.println("Error Reason: " + String(fbdo.errorReason().c_str()));
    }
}

void FirebaseManager::writeDigitalLed(bool state) {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for LED state write");
        return;
    }

    Serial.println("Writing LED state to Firebase: " + String(state ? "ON" : "OFF"));

    if (Firebase.setBool(fbdo, PATH_DIGITAL_LED, state)) {
        Serial.println("LED state successfully synchronized with Firebase");
    } else {
        Serial.println("Firebase Error: Failed to write LED state");
        Serial.println("Error Reason: " + String(fbdo.errorReason().c_str()));
    }
}

void FirebaseManager::uploadSensorData(float temperature, float humidity) {
    if (!isReady()) {
        Serial.println("Firebase Warning: Service not ready for sensor data upload");
        return;
    }
    
    Serial.println("Uploading sensor data to Firebase...");
    
    FirebaseJson sensorData;
    if (!isnan(temperature)) {
        sensorData.set("temperature", temperature);
        Serial.println("  Temperature data prepared: " + String(temperature) + " C");
    }
    if (!isnan(humidity)) {
        sensorData.set("humidity", humidity);
        Serial.println("  Humidity data prepared: " + String(humidity) + " %");
    }

    if (Firebase.updateNode(fbdo, PATH_SENSOR, sensorData)) {
        Serial.println("Sensor data successfully uploaded to Firebase");
    } else {
        Serial.println("Firebase Error: Failed to upload sensor data");
        Serial.println("Error Reason: " + String(fbdo.errorReason().c_str()));
    }
}

void FirebaseManager::printStatus() {
    Serial.println("Firebase Status:");
    Serial.println("  Connection: " + String(isReady() ? "Ready" : "Not Ready"));
}

bool FirebaseManager::isReady() {
    return Firebase.ready();
}