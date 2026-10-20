#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

// Your Advertisement Key (EID) from GoogleFindMyTools:
const char *eid_hex = "Your Advertisement Key (EID) from GoogleFindMyTools";

// Find My Device Network (FMDN) raw advertisement payload:
uint8_t adv_raw_data[29] = {
    0x02,   // Length
    0x01,   // Flags data type
    0x06,   // Flags (LE General Discoverable Mode, BR/EDR Not Supported)
    0x19,   // Length: 25 bytes
    0x16,   // Service data data type (16-bit UUID)
    0xAA,   // 16-bit Service UUID: 0xFEAA (Fast Pair / FMDN)
    0xFE,   
    0x41,   // FMDN Frame Type
    // Next 20 bytes: 20-byte Ephemeral Identifier (filled in setup)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x00    // Hashed flags
};

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n-------------------------------------------------");
    Serial.println("  ESP32 GOOGLE FIND MY DEVICE BEACON");
    Serial.println("-------------------------------------------------");

    // Convert hex string into raw 20 bytes
    for (int i = 0; i < 20; i++) {
        char byte_str[3] = { eid_hex[i * 2], eid_hex[i * 2 + 1], '\0' };
        adv_raw_data[8 + i] = (uint8_t)strtol(byte_str, NULL, 16);
    }

    // Initialize BLE using the standard Arduino ESP32 BLE library
    BLEDevice::init("Google Tracker");

    // Configure Advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    BLEAdvertisementData advData;
    advData.addData((char*)adv_raw_data, sizeof(adv_raw_data));
    
    pAdvertising->setAdvertisementData(advData);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinInterval(0x20); // ~20ms interval
    pAdvertising->setMaxInterval(0x20);

    // Start Advertising
    pAdvertising->start();

    Serial.println("=================================================");
    Serial.println(">>> SUCCESS: GOOGLE BEACON IS NOW LIVE ON AIR! <<<");
    Serial.print("Target Key: ");
    Serial.println(eid_hex);
    Serial.println("=================================================\n");
}

void loop() {
    delay(10000);
}
