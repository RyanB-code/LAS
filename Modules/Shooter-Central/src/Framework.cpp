#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS::Logging;

Framework::Framework() {
    window = std::make_shared<ShooterCentralWindow>(ShooterCentralWindow{});
}
Framework::~Framework(){

}

// MARK: Public Functions
bool Framework::setup(LAS::Logging::LoggerPtr setLoggerPtr, const std::string& directory){
    if(!setLogger(setLoggerPtr))
        return false;

    // -------------------------------
    // USE LOGGING ONCE LOGGER IS VERIFIED
    // -------------------------------

    if(!setupFilesystem(directory))
        return false;

   
   logger->log("Setup sucessful", Tags{"Routine", "SC"});
   return true;
}
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
// MARK: PRIVATE FUNCTIONS
bool Framework::setupFilesystem(std::string directory){
    if(directory.empty())
        return false;

    directory = LAS::TextManip::ensureSlash(directory);

    SubDirectories fileSystem;

    fileSystem.ammoDir         = directory + "Ammo/";
    fileSystem.drillsDir       = directory + "Drills/";
    fileSystem.eventsDir       = directory + "Events/";
    fileSystem.gunsDir         = directory + "Guns/";

    // Check paths are good
    if(!LAS::ensureDirectory(fileSystem.ammoDir)){
        logger->log("Error finding or creating directory [" + fileSystem.ammoDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(fileSystem.drillsDir)){
        logger->log("Error finding or creating directory [" + fileSystem.drillsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(fileSystem.eventsDir)){
        logger->log("Error finding or creating directory [" + fileSystem.eventsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(fileSystem.gunsDir)){
        logger->log("Error finding or creating directory [" + fileSystem.gunsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    return true;
}