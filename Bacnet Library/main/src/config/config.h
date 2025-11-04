#ifndef CONFIG_H
#define CONFIG_H

// System
const unsigned long DHT_UPLOAD_INTERVAL = 2000;
const unsigned long FIREBASE_POLL_INTERVAL = 1000;
const unsigned long STATUS_PRINT_INTERVAL = 10000;
const unsigned long BACNET_DISCOVERY_INTERVAL = 30000;
const unsigned long DEBOUNCE_DELAY = 50;

// Network
const unsigned long NETWORK_TIMEOUT = 15000;
const unsigned long SYSTEM_RESTART_DELAY = 15000;

// BACnet
#define BACNET_PORT 47808
#define DEVICE_ID 1010
#define VENDOR_ID 1110
#define MAX_APDU 1476

#endif