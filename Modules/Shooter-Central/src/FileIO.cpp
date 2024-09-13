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

    subDirectories.parentDir    = parentDir;
    subDirectories.ammoDir      = parentDir + "Ammo/";
    subDirectories.drillDir     = parentDir + "Drills/";
    subDirectories.eventDir     = parentDir + "Events/";
    subDirectories.gunDir       = parentDir + "Guns/";

    // Check paths are good
    if(!LAS::ensureDirectory(subDirectories.ammoDir)){
        logger->log("Error finding or creating directory [" + subDirectories.ammoDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(subDirectories.drillDir)){
        logger->log("Error finding or creating directory [" + subDirectories.drillDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(subDirectories.eventDir)){
        logger->log("Error finding or creating directory [" + subDirectories.eventDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(subDirectories.gunDir)){
        logger->log("Error finding or creating directory [" + subDirectories.gunDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

    return true;
}