#include "Framework.h"

using namespace ShooterCentral;

Framework::Framework(){

}
Framework::~Framework(){

}

bool Framework::addGun(GunPtr gun){
    if(!gun)
        return false;

    if(guns.contains(gun->getName()))
        return false;
    
    guns.try_emplace(gun->getName());
    return guns.contains(gun->getName());
}
bool Framework::removeGun(const std::string& key){
    if(!guns.contains(key))
        return true;

    guns.erase(key);
    return !guns.contains(key); // Return the inverse of contain()
}
void Framework::addDrill(Drill drill){
    drills.push_back(drill);
}
bool Framework::addAmmoToStockpile(uint64_t amount, AmmoPtr ammo){
    if(ammoStockpile.contains(ammo->name)){
        ammoStockpile.at(ammo->name).first += amount;
        return true;
    }
    else{
        ammoStockpile.try_emplace(ammo->name), std::pair(amount, ammo);
        return ammoStockpile.contains(ammo->name);
    }
}
bool Framework::removeAmmoFromStockPile (uint64_t amountUsed, const std::string& key){
    if(!ammoStockpile.contains(key))
        return true;
    
    auto& ammo {ammoStockpile.at(key)};

    if(ammo.first - amountUsed >= 100000)
        return false;
    else
        ammo.first -= amountUsed;

    return true;
}
