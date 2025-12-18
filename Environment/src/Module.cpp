#include "Module.h"

using namespace LAS;

Module::Module( LAS::ModuleFunctions::LoadModuleInfo    setLoadModuleInfo,
                LAS::ModuleFunctions::InitModule        setModuleInit,
                LAS::ModuleFunctions::VoidNoParams      setCleanup) :
        loadModuleInfoPtr   {setLoadModuleInfo},
        initModulePtr       {setModuleInit},
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
const ModuleInfo& Module::getModuleInfo() const{
    return moduleInfo;
}
std::string Module::getDirectory() const{
    return directory;
}
std::string Module::getRCFilePath() const{
    return rcFilePath;
}
std::vector<std::shared_ptr<Command>>& Module::getCommands(){
    return moduleInfo.commands;
}
void Module::cleanup(){
    Logging::setModuleTag(moduleInfo.shortTag); 
    if(cleanupPtr)
        cleanupPtr();
    Logging::setModuleTag("LAS"); 
}
bool Module::loadInfo(){
    if(!loadModuleInfoPtr(moduleInfo))
        return false;
    else
        return true;
}
bool Module::init(const EnvironmentInfo& envInfo){
    if(!initModulePtr(envInfo))
        return false;
   
    if(setDirectory(envInfo.directory) && setRCFilePath(envInfo.rcFilePath))
        return true;
    
    return false;
}
