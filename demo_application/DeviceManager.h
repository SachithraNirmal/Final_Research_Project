#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h>
#include "Config.h"

struct DeviceState {
    bool lightState = false;
    float lightBrightness = DEFAULT_LIGHT_BRIGHTNESS;
    float temperature = DEFAULT_TEMPERATURE;
    bool acState = false;
    String acMode = DEFAULT_AC_MODE;
    int fanSpeed = DEFAULT_FAN_SPEED;
};

class DeviceManager {
private:
    DeviceState state;

public:
    DeviceManager() {
        // Initialize GPIO pins
        pinMode(LIGHT_PIN, OUTPUT);
        pinMode(RELAY_PIN, OUTPUT);
        digitalWrite(LIGHT_PIN, LOW);
        digitalWrite(RELAY_PIN, LOW);
    }
    
    // Light control methods
    void setLightState(bool state) {
        this->state.lightState = state;
        updatePhysicalDevices();
        Serial.println(" Light " + String(state ? "turned ON" : "turned OFF"));
    }
    
    void setLightBrightness(float brightness) {
        this->state.lightBrightness = brightness;
        updatePhysicalDevices();
        Serial.println(" Brightness set to: " + String(brightness) + "%");
    }
    
    // AC control methods
    void setACState(bool state) {
        this->state.acState = state;
        updatePhysicalDevices();
        Serial.println(" AC " + String(state ? "turned ON" : "turned OFF"));
    }
    
    void setTemperature(float temperature) {
        this->state.temperature = temperature;
        Serial.println(" Temperature set to: " + String(temperature) + "°C");
    }
    
    void setACMode(const String& mode) {
        this->state.acMode = mode;
        Serial.println(" AC mode set to: " + mode);
    }
    
    void setFanSpeed(int speed) {
        this->state.fanSpeed = speed;
        Serial.println(" Fan speed set to: " + String(speed));
    }
    
    // Getters
    DeviceState getState() {
        return state;
    }
    
    bool getLightState() { return state.lightState; }
    float getLightBrightness() { return state.lightBrightness; }
    float getTemperature() { return state.temperature; }
    bool getACState() { return state.acState; }
    String getACMode() { return state.acMode; }
    int getFanSpeed() { return state.fanSpeed; }

private:
    void updatePhysicalDevices() {
        // Control LED based on light state and brightness
        if (state.lightState && state.lightBrightness > 0) {
            analogWrite(LIGHT_PIN, map(state.lightBrightness, 0, 100, 0, 1023));
        } else {
            digitalWrite(LIGHT_PIN, LOW);
        }
        
        // Control relay based on AC state
        digitalWrite(RELAY_PIN, state.acState ? HIGH : LOW);
    }
};

#endif