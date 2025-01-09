#pragma once
#include "esp_now.h"
// #include "network.h"
#include "client.h"

#define ENABLE_ENCRYPTION 0
#define PAYLOAD_BUFFER_SIZE 4

namespace Networking {
    class Server {
        private:
            Equipment::Machine* machine;
            Payload received_data_buffer[PAYLOAD_BUFFER_SIZE];
            Networking::Client rhs_client;
            Networking::Client lhs_client;
            
            static void on_data_receive(const uint8_t *mac, const uint8_t *incomingData, int len);
        public:
            Server(Equipment::Machine* machine);
            void init();
            bool add_peer(Equipment::Side side, uint8_t mac[6]);
            // void test_connection();

            void process_signal(Networking::Payload payload);

            // void synchronize();
            void tick();
    };
}