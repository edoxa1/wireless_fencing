#include "fencer.h"

Models::Fencer::Fencer(uint8_t id, Text fencer_name) {
    db_id = id;
    name = fencer_name;
}

Models::Fencer::Fencer()
{
    db_id = -1;
    name = "";
}

uint8_t Models::Fencer::get_id()
{
    return db_id;
}

Text Models::Fencer::get_name() {
    return name;
}

void Models::Fencer::set_name(Text new_name) {
    name = new_name;
}