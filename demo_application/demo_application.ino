#include "Config.h"
#include "WiFiManager.h"
#include "DeviceManager.h"
#include "WebServerManager.h"
#include "BACnet_ESP8266.h"

// Global instances
WiFiManager wifiManager;
DeviceManager deviceManager;
BACnet_ESP8266 bacnetController;
WebServerManager webServer(&deviceManager, &bacnetController);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n Starting BACnet ESP8266 Building Controller...");
    
    // Connect to WiFi
    if (!wifiManager.connect()) {
        Serial.println(" Failed to connect to WiFi. System halted.");
        while(1) delay(1000);
    }
    
    // Initialize BACnet controller
    if (bacnetController.begin(BACNET_DEVICE_INSTANCE)) {
        Serial.println(" BACnet controller initialized");
        
        // Create BACnet objects for building automation
        bacnetController.addObject(BACNET_OBJECT_BINARY_OUTPUT, LIGHT_OBJECT_ID, "Light_Switch", 0.0);
        bacnetController.addObject(BACNET_OBJECT_ANALOG_OUTPUT, LIGHT_BRIGHTNESS_OBJECT_ID, "Light_Brightness", 0.0);
        bacnetController.addObject(BACNET_OBJECT_ANALOG_INPUT, TEMPERATURE_OBJECT_ID, "Room_Temperature", 22.0);
        bacnetController.addObject(BACNET_OBJECT_BINARY_OUTPUT, AC_STATE_OBJECT_ID, "AC_State", 0.0);
        bacnetController.addObject(BACNET_OBJECT_ANALOG_OUTPUT, AC_MODE_OBJECT_ID, "AC_Mode", 1.0);
        bacnetController.addObject(BACNET_OBJECT_ANALOG_OUTPUT, FAN_SPEED_OBJECT_ID, "Fan_Speed", 2.0);
        
        Serial.println(" BACnet objects created");
    } else {
        Serial.println(" BACnet controller failed to start");
    }
    
    // Start web server
    webServer.begin();
    Serial.println(" Web interface ready: http://" + WiFi.localIP().toString());
    Serial.println(" BACnet Device ID: " + String(BACNET_DEVICE_INSTANCE));
    Serial.println(" System fully initialized and ready!");
}

void loop() {
    // Handle BACnet communications
    bacnetController.update();
    
    // Update BACnet objects with current device states
    DeviceState currentState = deviceManager.getState();
    bacnetController.updateBuildingObjects(
        currentState.lightState,
        currentState.lightBrightness,
        currentState.temperature,
        currentState.acState,
        currentState.acMode,
        currentState.fanSpeed
    );
    
    // Handle web clients
    webServer.handleClient();
    
    delay(100);
}