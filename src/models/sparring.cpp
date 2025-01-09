#include "sparring.h"

Models::Sparring::Sparring(Models::Fencer RHS, Models::Fencer LHS)
: RHS(RHS)
, LHS(LHS) {}

Models::Fencer &Models::Sparring::get_RHS_fencer()
{
    return RHS;
}

Models::Fencer &Models::Sparring::get_LHS_fencer()
{
    return LHS;
}

void Models::Sparring::initiate_fight()
{
    Serial.print("Starting a fight between "); 
    Serial.print(LHS.get_name()); 
    Serial.print(" and ");
    Serial.println(RHS.get_name());
}