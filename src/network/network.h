#pragma once
#include "equipment/machine.h"

namespace Networking {
    enum SignalType {
        GROUND,
        POINT
    };

    typedef struct Data {
        Equipment::Side side;
        SignalType signal_type;
        uint16_t relative_timestamp;
    } Data;

    typedef struct Payload {
        uint8_t mac[6];
        Data data;
        uint8_t len;
    } Payload;

    class Server;
    class Client;
}