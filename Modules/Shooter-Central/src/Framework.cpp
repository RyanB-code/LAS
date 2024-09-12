#include "Framework.h"

using namespace ShooterCentral;

Framework::Framework() {
    window = std::make_shared<ShooterCentralWindow>(ShooterCentralWindow{});
}
Framework::~Framework(){

}

// MARK: Public Functions
bool Framework::addGun(GunPtr gun){
    if(!gun)
        return false;

    if(guns.contains(gun->getName()))
        return false;
    
    return guns.try_emplace(gun->getName(), gun).second;
}
bool Framework::removeGun(const std::string& key){
    if(!guns.contains(key))
        return true;

    guns.erase(key);
    return !guns.contains(key); // Return the inverse of contain()
}
bool Framework::addDrill(DrillPtr drill){
     if(!drill)
        return false;

    std::string name { drill->name};

    if(drills.contains(name))
        return false;

    return drills.try_emplace(name, drill).second;
}
bool Framework::removeDrill(const std::string& key){
    if(!drills.contains(key))
        return true;

    drills.erase(key);
    return !drills.contains(key); // Return the inverse of contain()
}
bool Framework::addEvent(EventPtr event){
     if(!event)
        return false;

    std::string name { event->getName()};

    if(events.contains(name))
        return false;
    
    return events.try_emplace(name, event).second;
}
bool Framework::removeEvent(const std::string& key){
    if(!events.contains(key))
        return true;

    events.erase(key);
    return !events.contains(key); // Return the inverse of contain()
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
SCWindowPtr Framework::getWindow() const {
    return window;
}
std::string Framework::getCommandGroupName() const {
    return commandGroupName;
}
bool Framework::setLogger(LAS::Logging::LoggerPtr setLogger){
    if(!setLogger)
        return false;
    
    logger = setLogger;
    return true;
}
LAS::Logging::LoggerPtr Framework::getLogger() const {
    return logger;
}
