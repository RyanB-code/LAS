#include "Gun.h"

using namespace ShooterCentral;

Gun::Gun(std::string setName, WeaponType setWeaponType)
        :   name        { setName },
            weaponType  { setWeaponType }
{

}
Gun::~Gun(){

}
std::string Gun::getName() const{
    return name;
}
uint64_t Gun::getRoundCount() const{
    return totalRounds;
}
WeaponType Gun::getWeaponType() const{
    return weaponType;
}
void Gun::addToRoundCount(uint64_t add, const Ammo& ammoUsed){
    totalRounds += add;

    if(ammoTracker.contains(ammoUsed.name)){
        auto& ammo {ammoTracker.at(ammoUsed.name)};

        ammo.first += add;
    }
    else{
        ammoTracker.try_emplace(ammoUsed.name), std::pair(add, ammoUsed);
    }
}
