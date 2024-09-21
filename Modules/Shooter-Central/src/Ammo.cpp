#include "Ammo.h"

using namespace ShooterCentral;

// MARK: AMMO TRACKER
AmmoTracker::AmmoTracker(LAS::Logging::LoggerPtr setLogger): logger { setLogger }
{

}
AmmoTracker::~AmmoTracker(){
    
}
// MARK: STOCKPILE
bool AmmoTracker::addAmmoToStockpile (uint64_t amount, const std::string& key){
    if(ammoStockpile.contains(key)){
        ammoStockpile.at(key)->amount += amount;
        return true;
    }
    else
        return false;    
}
bool AmmoTracker::addNewAmmoToStockpile (AmmoPtr ammo){
    if(ammoStockpile.contains(ammo->name))
        return false;

    ammoStockpile.try_emplace(ammo->name, ammo);

    if(ammoStockpile.contains(ammo->name)){
        // Add to cartridges, does not matter return
        addCartridge(ammo->cartridge);
        return true;
    }
    else
        return false;
}
bool AmmoTracker::removeAmmoFromStockPile (uint64_t amountUsed, const std::string& key){
    if(!ammoStockpile.contains(key))
        return true;
    
    auto& ammo {ammoStockpile.at(key)};

    if(ammo->amount - amountUsed >= 100000)
        return false;
    else
        ammo->amount -= amountUsed;

    return true;
}
// MARK: GET INFO
void AmmoTracker::getAllAmmoNames(StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());


    for(const auto& pair : ammoStockpile){
        names.push_back(pair.second->name);
    }
}
void AmmoTracker::getAllAmmo (std::vector<Ammo>& list)   const{
     if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& a : ammoStockpile){
        list.push_back(*a.second);
    }
}

void AmmoTracker::getAllCartridgeNames(StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());


    for(const auto& pair : cartridges){
        names.push_back(pair.second);
    }
}
void AmmoTracker::getAmmoCountByCartridge (std::vector<std::pair<std::string, uint64_t>>& count) const{
    if(!count.empty())
        count.erase(count.begin(), count.end());

    StringVector namesList;
    getAllCartridgeNames(namesList); // Get all the names of cartridges

    // For every cartridge, add the amounts together
    for(const auto& s : namesList){
        int amountBuf { 0 };
        std::vector<Ammo> sameCartridgeList;
        getAllAmmoByCartridge(sameCartridgeList, s);

        for(const auto& a : sameCartridgeList)
            amountBuf += a.amount;

        count.push_back(std::pair{s, amountBuf});           // Add to list
    }
}
void AmmoTracker::getAllAmmoByCartridge(std::vector<Ammo>& list, const std::string& cartridgeName)   const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& pair : ammoStockpile){
        if(pair.second->cartridge == cartridgeName)
            list.push_back(*pair.second);
    }
}
bool AmmoTracker::addCartridge (const std::string& cartridge){
    if(cartridges.contains(cartridge))
        return false;

    cartridges.try_emplace(cartridge, cartridge);
    return cartridges.contains(cartridge);
}

// MARK: R/W AMMO
bool AmmoTracker::writeAllAmmo() const{
    using LAS::json;

    if(ammoStockpile.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Ammo objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }


	for(const auto& pair : ammoStockpile) {
        const auto& ammo = *pair.second;

        if(!AmmoHelper::writeAmmo(saveDirectory, ammo)) 
            logger->log("Directory [" + saveDirectory + "] was not found. Ammo [" + ammo.name + "] was not saved.", LAS::Logging::Tags{"ERROR", "SC"});
	}

    return true;
}
bool AmmoTracker::readAllAmmo(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    int filesThatCouldNotBeRead{0};
	const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			AmmoPtr ammo {std::make_shared<Ammo>(AmmoHelper::readAmmo(dirEntry.path().string()))};

            // Attempt adding to stockpile
            if(!addAmmoToStockpile(ammo->amount, ammo->name)){
                if(!addNewAmmoToStockpile(ammo)){
                    ++filesThatCouldNotBeRead;
                }
            }
		}
		catch(std::exception& e){
			++filesThatCouldNotBeRead;
		}
	}

	// Output number of files that could not be read
	if(--filesThatCouldNotBeRead > 0)    // There will always be the cartridges file that cannot be read, so subtracting that
		logger->log("Could not create Ammo object from file(s): " + filesThatCouldNotBeRead, LAS::Logging::Tags{"ROUTINE", "SC"});

	return true;
}
// MARK: R/W CARTRIDGE
bool AmmoTracker::writeAllCartridges() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += cartridgesFile;

    std::vector<std::string> rawCartridges;

    for(auto& c : cartridges){
        rawCartridges.push_back(c.second);
    }

    return AmmoHelper::writeAllCartridges(fullPath, rawCartridges);
}
std::string AmmoTracker::getDirectory() const{
    return saveDirectory;
}
bool AmmoTracker::readCartridges(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += cartridgesFile;

    StringVector cartridgeNames;
    AmmoHelper::readCartridges(fullPath, cartridgeNames);

    for(const auto& s : cartridgeNames){
        if(!addCartridge(s))
            logger->log("Cartridge named [" + s + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}

bool AmmoTracker::setDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}

// MARK: AMMO HELPER
bool AmmoHelper::writeAmmo(std::string directory, const Ammo& ammo){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return 2;
    

    // Make JSON
    json j;
    j["name"]           = ammo.name;
    j["manufacturer"]   = ammo.manufacturer;
    j["cartridge"]      = ammo.cartridge;
    j["grain"]          = int{ammo.grainWeight};
    j["amountOnHand"]   = ammo.amount;


    // Create JSON file name
    std::string fileName;
    for(const auto& c : ammo.name){     // Remove spaces and make lowercase
        if(isalpha(c))
            fileName += tolower(c);
        else if(c == ' ' || c == '\t')
            fileName += '-';
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
Ammo AmmoHelper::readAmmo(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    std::string nameBuf, manBuf, cartNameBuf; 
    uint8_t grainBuf;
    uint64_t amountBuf;

    j.at("name").get_to(nameBuf);
    j.at("manufacturer").get_to(manBuf);
    j.at("cartridge").get_to(cartNameBuf);
    j.at("grain").get_to(grainBuf);
    j.at("amountOnHand").get_to(amountBuf);

    return Ammo{nameBuf, manBuf, cartNameBuf, grainBuf, amountBuf };
}
bool AmmoHelper::writeAllCartridges(std::string path, const std::vector<std::string>& cartridges){
    using LAS::json;

    if(cartridges.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& cartridge : cartridges) 
        entries.push_back(cartridge);
    
    j["cartridges"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool AmmoHelper::readCartridges (std::string path, StringVector& cartridgeNames){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!cartridgeNames.empty())
        cartridgeNames.erase(cartridgeNames.begin(), cartridgeNames.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json cartridges;
	j.at("cartridges").get_to(cartridges);

	for (auto& elment : cartridges.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        cartridgeNames.push_back(nameBuf);
	}

    return true;
}
