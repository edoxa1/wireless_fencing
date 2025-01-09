#include "Arduino.h"
#include "machine.h"

#define POINT_DURATION_MILLIS 10
#define GROUND_DURATION_MILLIS 10

Equipment::Machine::Machine(short LHS_SIG_PIN, short LHS_GND_PIN, short RHS_SIG_PIN, short RHS_GND_PIN) 
: LHS_GND_PIN(LHS_GND_PIN) 
, LHS_SIG_PIN(LHS_SIG_PIN)
, RHS_GND_PIN(RHS_GND_PIN)
, RHS_SIG_PIN(RHS_SIG_PIN)
, active_sparring(false)
, weapon_type(Weapon::Type::EPEE)
, current_sparring(Models::Sparring(Models::Fencer(), Models::Fencer())) {}

void Equipment::Machine::init() {
    pinMode(LHS_GND_PIN, OUTPUT);
    pinMode(LHS_SIG_PIN, OUTPUT);
    pinMode(RHS_GND_PIN, OUTPUT);
    pinMode(RHS_SIG_PIN, OUTPUT);
}

void Equipment::Machine::point(Equipment::Side side, uint16_t timestamp) {
    // check side:
    uint8_t side_sig_pin = (side == Equipment::Side::LHS) ? LHS_SIG_PIN : RHS_SIG_PIN;
    // start timer to send short signal to machine
    uint32_t timer = millis();
    while(millis() - timer <= POINT_DURATION_MILLIS) {
        // sending signal here, for 'POINT_DURATION_MILLIS' milliseconds, the value defined above.
        // The value can be overriden from main sketch.
        digitalWrite(side_sig_pin, HIGH);
    }
    // stop sending signal
    digitalWrite(side_sig_pin, LOW);
}

void Equipment::Machine::ground(Equipment::Side side)
{
    uint8_t side_gnd_pin = (side == Equipment::Side::LHS) ? LHS_GND_PIN : RHS_GND_PIN;
    uint32_t timer = millis();
    while(millis() - timer <= GROUND_DURATION_MILLIS) {
        digitalWrite(side_gnd_pin, HIGH);
    }
    digitalWrite(side_gnd_pin, LOW);
}

void Equipment::Machine::test_pins() {
    uint32_t timer = millis();
    while (millis() - timer <= 1000) {
        point(Equipment::Side::LHS, 0);
        ground(Equipment::Side::LHS);
        point(Equipment::Side::RHS, 0);
        ground(Equipment::Side::RHS);
    }
}

void Equipment::Machine::initiate_sparring(Models::Sparring sparring) {
    this->active_sparring = true;
    this->current_sparring = sparring;
}
