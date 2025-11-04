#include "src/config/config.h"
#include "src/config/pins.h"
#include "src/config/credentials.h"
#include "src/BACnet/BACnetProtocol.h"
#include "src/Firebase/FirebaseManager.h"
#include "src/Sensors/SensorManager.h"
#include "src/DeviceControl/DeviceManager.h"
#include "src/Network/WiFiManager.h"

WiFiManager wifiManager;
FirebaseManager firebaseManager;
SensorManager sensorManager;
DeviceManager deviceManager;
BACnetProtocol bacnetProtocol;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== Smart Building Controller System Initialization ===");

  // Initialize all starts
  deviceManager.begin();
  sensorManager.begin();
  wifiManager.connect();
  firebaseManager.begin();
  bacnetProtocol.begin();

  // Initial data synchronization
  firebaseManager.syncInitialData();

  Serial.println("=== System Initialization Complete ===");
  Serial.println("Smart Building Controller is now operational");
  Serial.println("Device Name: SBMCon, Device ID: 1010, Vendor: Sachithra");
  Serial.println("BACnet Protocol: Enabled and Listening on Port 47808");
  Serial.println("Firebase Integration: Active and Synchronized");
  Serial.println("Manual Control: Button input enabled");
  Serial.println("======================================");
}

void loop() {
  unsigned long currentTime = millis();

  // Handle all system tasks
  deviceManager.handleButton();
  bacnetProtocol.handle();
  firebaseManager.syncData();
  sensorManager.readAndUploadData();

  // Periodic tasks
  bacnetProtocol.broadcastPresence();
  printSystemStatus(currentTime);

  delay(10);
}

void printSystemStatus(unsigned long currentTime) {
  static unsigned long lastStatusPrint = 0;
  
  if (currentTime - lastStatusPrint >= STATUS_PRINT_INTERVAL) {
    lastStatusPrint = currentTime;
    
    Serial.println("=== System Status Report ===");
    deviceManager.printStatus();
    sensorManager.printStatus();
    bacnetProtocol.printStatus();
    wifiManager.printStatus();
    firebaseManager.printStatus();
    Serial.println("=== End Status Report ===");
  }
}