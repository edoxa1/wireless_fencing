#pragma once
#include <StringUtils.h>

namespace Models {
    class Fencer {
        private:
            uint8_t db_id;
            Text name;
        public:
            Fencer(uint8_t id, Text fencer_name);
            Fencer();
            uint8_t get_id();
            
            Text get_name();
            void set_name(Text new_name);
    };
}