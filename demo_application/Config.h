#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
const char* WIFI_SSID = "Sachithra_4G";
const char* WIFI_PASSWORD = "Sachi@4G";

// Server Configuration
const int WEB_SERVER_PORT = 80;

// BACnet Configuration
const uint32_t BACNET_DEVICE_INSTANCE = 12345;

// BACnet Object IDs
const uint32_t LIGHT_OBJECT_ID = 1001;
const uint32_t LIGHT_BRIGHTNESS_OBJECT_ID = 1002;
const uint32_t TEMPERATURE_OBJECT_ID = 2001;
const uint32_t AC_STATE_OBJECT_ID = 3001;
const uint32_t AC_MODE_OBJECT_ID = 3002;
const uint32_t FAN_SPEED_OBJECT_ID = 3003;

// GPIO Pin Configuration
const int LIGHT_PIN = 5;   // D1 on NodeMCU
const int RELAY_PIN = 4;   // D2 on NodeMCU

// Device Default Values
const float DEFAULT_TEMPERATURE = 22.0;
const float DEFAULT_LIGHT_BRIGHTNESS = 0.0;
const String DEFAULT_AC_MODE = "cool";
const int DEFAULT_FAN_SPEED = 2;

#endif