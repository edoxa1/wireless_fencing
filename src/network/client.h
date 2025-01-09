#pragma once
#include "esp_now.h"
#include "network.h"

namespace Networking {
    typedef struct SyncData {
        uint16_t server_time;
    } SyncData;

    class Client {
        private:
            Equipment::Side side;
            uint8_t mac[6];
            static void on_data_send(const uint8_t *mac_addr, esp_now_send_status_t status);

        public:
            Client();
            Client(Equipment::Side side, u_int8_t mac[6]);
            void sync();
            void test_connection();
    };
}