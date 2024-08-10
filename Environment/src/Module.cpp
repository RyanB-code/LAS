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

std::string Module::getTitle() const{
    return moduleInfo.title;
}
std::string Module::getGroupName() const{
    return moduleInfo.commandGroupName;
}
const ModuleInfo& Module::getInfo() const{
    return moduleInfo;
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
    else
        return true;
}