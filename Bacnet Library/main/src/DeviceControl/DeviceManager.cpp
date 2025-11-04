#include <Arduino.h>
#include "DeviceManager.h"

void DeviceManager::begin() {
    Serial.println("Initializing device control hardware...");
    initializePins();
    setDigitalLed(false);
    setLEDBrightness(0);
}

void DeviceManager::initializePins() {
    pinMode(LED_BO, OUTPUT);
    pinMode(DIM_LED_AO, OUTPUT);
    pinMode(BUTTON_BI, INPUT_PULLUP);
    
    digitalWrite(LED_BO, LOW);
    analogWrite(DIM_LED_AO, 0);
    
    analogWriteRange(255);
    analogWriteFreq(1000);
}

void DeviceManager::handleButton() {
    bool currentButtonState = digitalRead(BUTTON_BI);
    
    // Detect button press with debounce
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
            Serial.println("Button Press Detected - Processing Toggle Request");
            
            // Toggle LED state
            bool newLedState = !ledState;
            setDigitalLed(newLedState);
            
            Serial.println("Button action completed:");
            Serial.println("  LED Toggled to: " + String(newLedState ? "ON" : "OFF"));
            
            lastDebounceTime = millis();
        }
    }
    
    lastButtonState = currentButtonState;
}

void DeviceManager::setDigitalLed(bool enabled) {
    ledState = enabled;
    digitalWrite(LED_BO, ledState ? HIGH : LOW);
    
    Serial.println("Digital LED state changed:");
    Serial.println("  Hardware: " + String(enabled ? "ON" : "OFF"));
}

void DeviceManager::setLEDBrightness(uint8_t brightness) {
    if (brightness > 255) {
        brightness = 255;
        Serial.println("Brightness Warning: Value clamped to maximum 255");
    }
    currentBrightness = brightness;
    analogWrite(DIM_LED_AO, brightness);
    Serial.println("PWM LED brightness set to: " + String(brightness) + "/255");
}

void DeviceManager::printStatus() {
    Serial.println("Device Control Status:");
    Serial.println("  Digital LED: " + String(ledState ? "ON" : "OFF"));
    Serial.println("  Brightness Level: " + String(currentBrightness) + "/255");
}

bool DeviceManager::getLedState() { return ledState; }
uint8_t DeviceManager::getCurrentBrightness() { return currentBrightness; }