#pragma once
#include <Arduino.h>
#include "server.h"
#include "client.h"


Networking::Server *server_instance = nullptr;
volatile uint8_t buffer_index = 0;
volatile uint8_t buffer_count = 0;

Networking::Server::Server(Equipment::Machine* machine)
: machine(machine) {}


void Networking::Server::on_data_receive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    Server *instance = server_instance; 
    if (instance == nullptr) {
        Serial.println("Server is not initialized! Use 'server.init()'");
        return;
    }

    if (buffer_index == PAYLOAD_BUFFER_SIZE - 1) {
        Serial.println("Overriding oldest buffer");
        buffer_index = 0;
    }

    Networking::Data data;
    Networking::Payload payload;
    memcpy(&payload.mac, mac, 6);
    memcpy(&data, incomingData, len);
    payload.data = data; payload.len = len;

    Serial.print("RECEIVED: Size of packet: "); Serial.print(len); Serial.print(". uint8+uint16=3bytes");
    instance->received_data_buffer[buffer_index] = payload;
    buffer_count++;
}

void Networking::Server::init()
{
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    server_instance = this;
    esp_now_register_recv_cb(on_data_receive);
}

bool Networking::Server::add_peer(Equipment::Side side, uint8_t mac[6]) {
    // Register peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return false;
    }

    Networking::Client client(side, mac);
    if (side == Equipment::Side::LHS) {
        lhs_client = client;
        return true;
    }

    rhs_client = client;
    return true;
}

void Networking::Server::process_signal(Networking::Payload payload) {
    // maybe implement check for side, e.g. if (client.side == payload.side)
    if (payload.data.signal_type == Networking::SignalType::POINT) {
        machine->point(payload.data.side, payload.data.relative_timestamp);
    }
    else if (payload.data.signal_type == Networking::SignalType::GROUND) {
        machine->ground(payload.data.side);
    }
}

void Networking::Server::tick() {
    if (buffer_count == 0) return;
    for (uint8_t i = 0; i < buffer_count; i++) {
        Networking::Payload payload = received_data_buffer[i];
        process_signal(payload);
    }
}
