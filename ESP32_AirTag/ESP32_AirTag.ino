#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"

// =========================================================================
// REPLACE THIS WITH THE PUBLIC KEY GENERATED IN STEP 1:
// =========================================================================
uint8_t public_key[28] = { 0x99, 0xE3, 0xF0, 0x4A, 0x37, 0x03, 0x8F, 0x13, 0xA6, 0xEB, 0xD1, 0x1A, 0x85, 0x6C, 0x93, 0x07, 0x90, 0x9C, 0xAA, 0xC8, 0x8A, 0x1E, 0xB2, 0xA5, 0xBF, 0x04, 0x97, 0x3F };
// =========================================================================

// Apple Find My Advertising Parameters
static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x0640, // ~1000ms advertisement interval
    .adv_int_max        = 0x0640,
    .adv_type           = ADV_TYPE_NONCONN_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .peer_addr          = {0},
    .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Raw Apple Find My BLE advertisement packet (31 bytes max)
static uint8_t raw_adv_data[31];

void set_addr_from_key(esp_bd_addr_t addr, uint8_t *pubkey) {
    // Sets the two MSBs to 1 for a valid BLE random static address
    addr[0] = pubkey[0] | 0b11000000;
    addr[1] = pubkey[1];
    addr[2] = pubkey[2];
    addr[3] = pubkey[3];
    addr[4] = pubkey[4];
    addr[5] = pubkey[5];
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting ESP32 AirTag Beacon...");

    // Build the Apple Find My Advertisement Packet:
    raw_adv_data[0] = 0x1E;       // Length: 30 bytes
    raw_adv_data[1] = 0xFF;       // Type: Manufacturer Specific Data
    raw_adv_data[2] = 0x4C;       // Apple Company ID (0x004C) - Byte 1
    raw_adv_data[3] = 0x00;       // Apple Company ID - Byte 2
    raw_adv_data[4] = 0x12;       // Offline Finding Type
    raw_adv_data[5] = 0x19;       // Payload Length: 25 bytes
    raw_adv_data[6] = 0x00;       // Status Byte / Battery (0x00 = Normal)

    // Copy the first 22 bytes of public key
    memcpy(&raw_adv_data[7], &public_key[6], 22);

    // Append hint bits
    raw_adv_data[29] = public_key[0] >> 6;
    raw_adv_data[30] = 0x00;

    // Initialize Bluetooth Controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();

    // Derive and set Random Static MAC address matching the key
    esp_bd_addr_t random_mac;
    set_addr_from_key(random_mac, public_key);
    esp_ble_gap_set_rand_addr(random_mac);

    // Configure and start advertising
    esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
    esp_ble_gap_start_advertising(&ble_adv_params);

    Serial.println("ESP32 AirTag is now broadcasting!");
}

void loop() {
    // The ESP32 Bluetooth hardware handles broadcasting automatically in the background
    delay(5000);
}
