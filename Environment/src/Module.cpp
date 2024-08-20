#include "Module.h"

using namespace LAS;

Module::Module( const LoggerPtr&             setLogger,
                LAS::Modules::LoadModuleInfo            setLoadModuleInfo,
                LAS::Modules::LoadEnvironmentInfo       setLoadEnvironmentInfo,
                LAS::Modules::VoidNoParams              setCleanup) :
        logger              {setLogger},
        loadModuleInfoPtr   {setLoadModuleInfo},
        loadEnvInfoPtr      {setLoadEnvironmentInfo},
        cleanupPtr          {setCleanup}
{

}
Module::~Module(){
    cleanup();
}
bool Module::setDirectory(std::string directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);              // Ensure slash at the end

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    directory = qualifiedDirectory;
    return true;
}
bool Module::setRCFilePath(const std::string& path){
    if(!std::filesystem::exists(path))
        return false;
    
    rcFilePath = path;
    return true;
}
const Version& Module::getSDKVersion() const{
    return moduleInfo.sdkVersion;
}

std::string Module::getTitle() const{
    return moduleInfo.title;
}
std::string Module::getGroupName() const{
    return moduleInfo.commandGroupName;
}
std::string Module::getDirectory() const{
    return directory;
}
std::string Module::getRCFilePath() const{
    return rcFilePath;
}
WindowPtr Module::getWindow() const{
    return moduleInfo.window;
}
std::vector<CommandPtr>& Module::getCommands(){
    return moduleInfo.commands;
}
void Module::cleanup(){
    if(cleanupPtr)
        cleanupPtr();
}
bool Module::loadModuleInfo(){
    if(!loadModuleInfoPtr(moduleInfo))
        return false;
    else
        return true;
}
bool Module::loadEnvInfo(const EnvironmentInfo& envInfo){
    if(!loadEnvInfoPtr(envInfo))
        return false;
   
    if(setDirectory(envInfo.directory) && setRCFilePath(envInfo.rcFilePath))
        return true;
    
    return false;
}