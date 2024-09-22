#include "Gun.h"

using namespace ShooterCentral;

// MARK: GUN
Gun::Gun(std::string setName, WeaponType setWeaponType, std::string setCartridge)
        :   name        { setName },
            weaponType  { setWeaponType },
            cartridge   { setCartridge }
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
std::string Gun::getCartridge () const{
    return cartridge;
}

bool Gun::addNewAmmoTypeToRoundCount(TrackedAmmoPtr ammo){
    if(!ammo)
        return false;

    if(ammoTracker.contains(ammo->ammoType.name))
        return false;

    totalRounds += ammo->amount;
    return ammoTracker.try_emplace(ammo->ammoType.name, ammo).second;
}
bool Gun::addToRoundCount(const std::string& ammoName, uint64_t amount){
    if(!ammoTracker.contains(ammoName))
        return false;
    
    ammoTracker.at(ammoName)->amount += amount;
    return true;
}

void Gun::getAllAmmoUsed(std::vector<TrackedAmmo>& ammoUsed) const{
    if(!ammoUsed.empty())
        ammoUsed.erase(ammoUsed.begin(), ammoUsed.end());


    for(const auto& pair : ammoTracker){
        ammoUsed.push_back(*pair.second);
    }
}


// MARK: GUN TRACKER
GunTracker::GunTracker(LAS::Logging::LoggerPtr setLogger): logger { setLogger }
{

}
GunTracker::~GunTracker(){
    
}
bool GunTracker::setDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}
std::string GunTracker::getDirectory() const{
    return saveDirectory;
}
bool GunTracker::addGun(GunPtr gun){
    if(!gun)
        return false;

    if(guns.contains(gun->getName()))
        return false;
    
    return guns.try_emplace(gun->getName(), gun).second;
}
bool GunTracker::removeGun(const std::string& key){
    if(!guns.contains(key))
        return true;

    guns.erase(key);
    return !guns.contains(key); // Return the inverse of contain()
}
GunPtr GunTracker::getGun(const std::string& key) const{
    if(!guns.contains(key))
        return nullptr;

    return guns.at(key);
}

// MARK: GUN HELPER
bool GunHelper::writeGun(std::string directory, const Gun& gun){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    
    // Make JSON
    json j;
    j["name"]           = gun.getName();
    j["weaponType"]     = GunHelper::weaponTypeToStr(gun.getWeaponType());

    // Entries in ammo tracker

    // Create JSON file name
    std::string fileName;
    for(const auto& c : gun.getName()){     // Remove spaces and make lowercase
        if(isalpha(c))
            fileName += tolower(c);
        else if(c == ' ' || c == '\t')
            fileName += '_';
        else if(isalnum(c))
            fileName += c;
    }
    fileName += ".json";

    // Make fully qualified path
    std::string filePath;
    filePath = directory + fileName;

    // Write to file
    std::ofstream file{filePath};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
Gun GunHelper::readGun(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    std::string nameBuf, weaponTypeBuf; 
    uint64_t roundCountBuf;

    j.at("name").get_to(nameBuf);
    j.at("roundCount").get_to(roundCountBuf);
    j.at("weaponType").get_to(weaponTypeBuf);

    Gun gunBuf { nameBuf, GunHelper::strToWeaponType(weaponTypeBuf)};
    //gunBuf.addToRoundCount(roundCountBuf);

    return gunBuf;
}
std::string GunHelper::weaponTypeToStr(const WeaponType& type){
    std::string weaponTypeText;
    switch(type){
        case WeaponType::PISTOL:
            weaponTypeText = "PISTOL";
            break;
        case WeaponType::RIFLE:
            weaponTypeText = "RIFLE";
            break;
        case WeaponType::PRECISION_RIFLE:
            weaponTypeText = "PRECISION RIFLE";
            break;
        case WeaponType::NOT_APPLICABLE:
            weaponTypeText = "N/A";
            break;
        default:
            weaponTypeText = "UNHANDLED CASE";
            break;
    }
    return weaponTypeText;
}
WeaponType  GunHelper::strToWeaponType(const std::string& string){
    WeaponType buffer;

    if(string == "PISTOL")
        buffer = WeaponType::PISTOL;
    else if(string == "RIFLE")
        buffer = WeaponType::RIFLE;
    else if(string == "PRECISION RIFLE")
        buffer = WeaponType::PRECISION_RIFLE;
    else
        buffer = WeaponType::NOT_APPLICABLE;
    
    return buffer;
}

