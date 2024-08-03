#include "Module.h"

using namespace LAS;

Module::Module( const LoggerPtr&             setLogger,
                LAS::Modules::loadFunction   setLoad, 
                LAS::Modules::voidNoParams   setCleanup) :
        logger      {setLogger},
        loadPtr     {setLoad},
        cleanupPtr  {setCleanup}
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




bool Module::load(const EnvironmentInfo& whatToPass){
    if(!loadPtr(whatToPass, moduleInfo))
        return false;

    if(moduleInfo.title.empty() || moduleInfo.commandGroupName.empty())
        return false;

    return true;
}
void Module::cleanup(){
    if(cleanupPtr)
        cleanupPtr();
}
