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

    Filesystem filesystem {directory};

    if(!setupFilesystem(filesystem)){
        logger->log("Failed to setup filesystem", Tags{"FATAL", "SC"});
        return false;
    }

    if(!setupAmmoTracker(filesystem.ammoDir)){
        logger->log("Failed to setup Ammo Tracker", Tags{"FATAL", "SC"});
        return false;
    }

    if(!setupWindow()){
        logger->log("Failed to setup window", Tags{"FATAL", "SC"});
        return false;
    }

   
    logger->log("Setup sucessful", Tags{"Routine", "SC"});
    return true;
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
bool Framework::setupFilesystem(Filesystem& filesystem){
    if(filesystem.parentDir.empty())
        return false;

    filesystem.parentDir = LAS::TextManip::ensureSlash(filesystem.parentDir);

    filesystem.ammoDir         = filesystem.parentDir + "Ammo/";
    filesystem.drillsDir       = filesystem.parentDir + "Drills/";
    filesystem.eventsDir       = filesystem.parentDir + "Events/";
    filesystem.gunsDir         = filesystem.parentDir + "Guns/";

    // Check paths are good
    if(!LAS::ensureDirectory(filesystem.ammoDir)){
        logger->log("Error finding or creating directory [" + filesystem.ammoDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.drillsDir)){
        logger->log("Error finding or creating directory [" + filesystem.drillsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.eventsDir)){
        logger->log("Error finding or creating directory [" + filesystem.eventsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.gunsDir)){
        logger->log("Error finding or creating directory [" + filesystem.gunsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    return true;
}
bool Framework::setupAmmoTracker(std::string directory){
    if(!ammoTracker)
        ammoTracker = std::make_shared<AmmoTracker>(logger);

    if(!ammoTracker->setDirectory(directory))
        return false;
    
    if(!ammoTracker->readCartridges())
        return false;

    if(!ammoTracker->readAllAmmo())
        return false;

    return true;
}
bool Framework::setupWindow(){
    if(!window->addAmmoTracker(ammoTracker)){
        logger->log("Could not add Ammo Tracker to window", LAS::Logging::Tags{"FATAL", "SC"});
        return false;
    }

    return true;
}
