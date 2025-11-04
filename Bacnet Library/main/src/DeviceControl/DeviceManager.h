#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <Arduino.h> 
#include "../config/pins.h"
#include "../config/config.h"

class DeviceManager {
public:
    void begin();
    void handleButton();
    void printStatus();
    void setDigitalLed(bool on);
    void setLEDBrightness(uint8_t brightness);
    bool getLedState();
    uint8_t getCurrentBrightness();

private:
    bool ledState = false;
    bool lastButtonState = HIGH;
    uint8_t currentBrightness = 0;
    unsigned long lastDebounceTime = 0;
    
    void initializePins();
};

#endif