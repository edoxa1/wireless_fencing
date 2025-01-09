#pragma once
#include "fencer.h"

namespace Models {
    class Sparring {
        private:
            Models::Fencer RHS;
            Models::Fencer LHS;
        public:
            Sparring(Models::Fencer RHS, Models::Fencer LHS);

            Models::Fencer& get_RHS_fencer();
            Models::Fencer& get_LHS_fencer();

            void initiate_fight();
    };   
}