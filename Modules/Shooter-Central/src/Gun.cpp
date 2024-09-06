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
bool Gun::addToRoundCount(AmmoPtr ammoType, uint64_t roundsShot){
    if(!ammoType || roundsShot == 0)
        return false;

    totalRounds += roundsShot;

    if(ammoTracker.contains(ammoType->name)){
        ammoTracker.at(ammoType->name).second += roundsShot; // Add to existing ammo type already used
        return true;
    }
    else
        return ammoTracker.try_emplace(ammoType->name), std::pair{ammoType, roundsShot}.second;
}
