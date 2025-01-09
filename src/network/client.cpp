#include "client.h"

void Networking::Client::on_data_send(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

Networking::Client::Client() {
    // Initialize default values
    memset(mac, 0, sizeof(mac));  // Set MAC to 0
    side = Equipment::Side::UNKNOWN;  // Set side to a default value
}

Networking::Client::Client(Equipment::Side side, u_int8_t mac[6])
{
    memcpy(this->mac, mac, 6); // set new MAC
    this->side = side; // set side
    esp_now_register_send_cb(on_data_send); // register callback for sending
}

void Networking::Client::sync()
{
}

void Networking::Client::test_connection() {
    // CLIENT DATA SHOUDL BE TEST DATA, NOT Networking::Data data;
    Networking::SyncData data;
    data.server_time = (uint16_t) millis();

    esp_err_t lhs_result = esp_now_send(mac, (uint8_t *) &data, sizeof(data));
    if (lhs_result != ESP_OK) {
        Serial.print("LHS test failed. Error code: "); 
        Serial.println(lhs_result);
    }
    esp_err_t rhs_result = esp_now_send(mac, (uint8_t *) &data, sizeof(data));
    if (lhs_result != ESP_OK) {
        Serial.print("RHS test failed. Error code: "); 
        Serial.println(rhs_result);
    }
    
    Serial.println("ESP-NOW test successfull");
}
