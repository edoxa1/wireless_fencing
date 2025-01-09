#pragma once
#include "models/sparring.h"
#include "weapon.h"

namespace Equipment {
    enum Side {
        UNKNOWN,
        LHS,
        RHS
    };

    class Machine {
        private:
            const short LHS_GND_PIN; // GROUND PIN
            const short LHS_SIG_PIN; // SIGNAL PIN

            const short RHS_GND_PIN;
            const short RHS_SIG_PIN;

            Weapon::Type weapon_type;

            bool active_sparring;
            Models::Sparring current_sparring;
            // TODO: INTRODUCE TIMESTAMP & SYNCHRONIZATION
        public:
            Machine(short LHS_SIG_PIN, short LHS_GND_PIN, short RHS_SIG_PIN, short RHS_GND_PIN); // for epee
            void init();
            void initiate_sparring(Models::Sparring sparring);        

            void point(Equipment::Side side, uint16_t timestamp);
            void ground(Equipment::Side side);

            void test_pins();
    };
}