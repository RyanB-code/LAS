#include "Gun.h"

using namespace ShooterCentral;

// MARK: GUN
Gun::Gun(std::string setName, WeaponType setWeaponType, std::string setCartridge, uint16_t setID)
        :   name        { setName },
            weaponType  { setWeaponType },
            cartridge   { setCartridge },
            ID          { setID }
{

}
Gun::~Gun(){

}
std::string Gun::getName() const{
    return name;
}
uint64_t Gun::getRoundCount() const{
    uint64_t rounds { 0 };
    for(const auto& pair : ammoTracker){
        rounds += pair.second->amount;
    }

    return rounds;
}
WeaponType Gun::getWeaponType() const{
    return weaponType;
}
std::string Gun::getCartridge () const{
    return cartridge;
}
uint16_t Gun::getID() const{
    return ID;
}
bool Gun::addToRoundCount(TrackedAmmoPtr ammo){
    if(!ammo)
        return false;
    
    if(ammoTracker.contains(ammo->ammoType.name))
        return addToRoundCount(ammo->ammoType.name, ammo->amount);

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
// MARK: GUN FACTORY
uint16_t GunFactory::gunIDNumber = 0;
GunFactory::GunFactory()
{

}
GunFactory::~GunFactory(){
    
}
Gun GunFactory::createGun (const std::string& name, WeaponType weaponType, std::string cartridge) const{
    ++gunIDNumber;

    if(gunIDNumber > 100)
        throw std::out_of_range("Gun total reached");

    return Gun{ name, weaponType, cartridge, gunIDNumber};
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
// MARK: CREATE GUNS
uint16_t GunTracker::createPistol(const std::string& name, const std::string& cartridge){
    try{
        Gun gunBuf { gunFactory.createGun(name, WeaponType::PISTOL, cartridge)};

        if(addGun(std::make_shared<Gun>(gunBuf)))
            return gunBuf.getID();
        else
            return 0;
    }
    catch(std::exception& e){
        return 0;
    }
}
uint16_t GunTracker::createRifle(const std::string& name, const std::string& cartridge){
    try{
        Gun gunBuf { gunFactory.createGun(name, WeaponType::RIFLE, cartridge)};

        if(addGun(std::make_shared<Gun>(gunBuf)))
            return gunBuf.getID();
        else
            return 0;
    }
    catch(std::exception& e){
        return 0;
    }
}
uint16_t GunTracker::createPrecisionRifle (const std::string& name, const std::string& cartridge){
    try{
        Gun gunBuf { gunFactory.createGun(name, WeaponType::PRECISION_RIFLE, cartridge)};

        if(addGun(std::make_shared<Gun>(gunBuf)))
            return gunBuf.getID();
        else
            return 0;
    }
    catch(std::exception& e){
        return 0;
    }
}
bool GunTracker::removeGun(const uint16_t& ID){
    if(!guns.contains(ID))
        return true;

    guns.erase(ID);
    return !guns.contains(ID); // Return the inverse of contain()
}
// MARK: GET INFO
uint64_t GunTracker::getGunTotal() const{
    return guns.size();
}
GunPtr GunTracker::getGun(const uint16_t& ID) const{
    if(!guns.contains(ID))
        return nullptr;

    return guns.at(ID);
}
void GunTracker::getAllGunIDs(std::vector<uint16_t>& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& pair : guns)
        list.push_back(pair.first);
}
// MARK: R/W Guns
bool GunTracker::writeAllGuns() const{
    using LAS::json;

    if(guns.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Ammo objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

	for(const auto& pair : guns) {
        const auto& gun = *pair.second;

        if(!GunHelper::writeGun(saveDirectory, gun)) 
            logger->log("Directory [" + saveDirectory + "] was not found. Ammo [" + gun.getName() + "] was not saved.", LAS::Logging::Tags{"ERROR", "SC"});
	}

    return true;
}
bool GunTracker::readGuns(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    int filesThatCouldNotBeRead{0};
	const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			GunPtr gunBuf {std::make_shared<Gun>(GunHelper::readGun(dirEntry.path().string()))};

            if(!addGun(gunBuf))
                ++filesThatCouldNotBeRead;
		}
		catch(std::exception& e){
			++filesThatCouldNotBeRead;
		}
	}

	// Output number of files that could not be read
	if(--filesThatCouldNotBeRead > 0)    // There will always be the cartridges file that cannot be read, so subtracting that
		logger->log("Could not create Gun object from file(s): " + filesThatCouldNotBeRead, LAS::Logging::Tags{"ROUTINE", "SC"});

	return true;
}
// MARK: PRIVATE FUNCTIONS
bool GunTracker::addGun(GunPtr gun){
    if(!gun)
        return false;

    if(guns.contains(gun->getID()))
        return false;
    
    return guns.try_emplace(gun->getID(), gun).second;
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
    j["cartridge"]      = gun.getCartridge();

    // Write every ammo type used and amount
    std::vector<TrackedAmmo> ammoUsed;
    gun.getAllAmmoUsed(ammoUsed);

	nlohmann::json trackedAmmoArray = nlohmann::json::array();
	for (const auto& pair : ammoUsed){
        json trackedAmmoJson { AmmoHelper::ammoTypeToJson(pair.ammoType), pair.amount };
        trackedAmmoArray.push_back(trackedAmmoJson);
    }

	j["trackedAmmo"] = trackedAmmoArray;


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

    std::string nameBuf, weaponTypeBuf, cartBuf; 

    j.at("name").get_to(nameBuf);
    j.at("weaponType").get_to(weaponTypeBuf);
    j.at("cartridge").get_to(cartBuf);

    GunFactory gunFactory;

    Gun gunBuf { gunFactory.createGun(nameBuf, GunHelper::strToWeaponType(weaponTypeBuf), cartBuf) };
    
    nlohmann::json trackedAmmoList;
	j.at("trackedAmmo").get_to(trackedAmmoList);
	
    // Add for each element
	for (auto& trackedAmmoListElement : trackedAmmoList.items()) {
		nlohmann::json pair = trackedAmmoListElement.value();

        uint64_t amountBuf { 0 };
		AmmoType ammoTypeBuf {AmmoHelper::jsonToAmmoType(pair.at(0))};
		pair.at(1).get_to(amountBuf);
        
        gunBuf.addToRoundCount(std::make_shared<TrackedAmmo>(ammoTypeBuf, amountBuf));
	}

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

