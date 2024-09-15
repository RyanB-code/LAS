#include "FileIO.h"

using namespace ShooterCentral;

FileIO::FileIO(LAS::Logging::LoggerPtr setLogger) 
        :   logger          { setLogger }
{

}
FileIO::~FileIO(){

}
std::string FileIO::getSaveDirectory() const{
    return saveDirectory;
}
bool FileIO::setSaveDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}
bool FileIO::setupSubDirectories(std::string parentDir){

    if(parentDir.empty()){
        if(saveDirectory.empty())
            return false;
        
        parentDir = LAS::TextManip::ensureSlash(saveDirectory);
    }

    if(parentDir.empty())
        return false;

    fileSystem.parentDir        = parentDir;
    fileSystem.ammoFile         = parentDir + "Ammo.json";
    fileSystem.drillsFile       = parentDir + "Drills.json";
    fileSystem.eventsFile       = parentDir + "Events.json";
    fileSystem.gunsFile         = parentDir + "Guns.json";

    // Check paths are good
    if(!LAS::ensureFile(fileSystem.ammoFile)){
        logger->log("Error finding or creating file [" + fileSystem.ammoFile + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureFile(fileSystem.drillsFile)){
        logger->log("Error finding or creating file [" + fileSystem.drillsFile + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureFile(fileSystem.eventsFile)){
        logger->log("Error finding or creating file [" + fileSystem.eventsFile + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureFile(fileSystem.gunsFile)){
        logger->log("Error finding or creating file [" + fileSystem.gunsFile + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

    return true;
}

// MARK: READ & WRITE
bool FileIO::writeAmmo(const std::vector<Ammo>& ammoList) const{
    using LAS::json;

    if(!std::filesystem::exists(fileSystem.ammoFile))
		return false;

    if(ammoList.empty())
        return true;

    json j { 
        {"ammo types", ammoList.size()}
    };

	// Print all ammo in the vector
	nlohmann::json entries = nlohmann::json::array();
	for (const auto& ammo : ammoList) {

		entries.push_back(ammo.name);
        entries.push_back(ammo.manufacturer);
        entries.push_back(ammo.cartridgeType.getName());
        entries.push_back(ammo.grainWeight);
	}

	j["ammo types"] = entries;
	
	// Write to file 
    std::ofstream file{fileSystem.ammoFile};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}