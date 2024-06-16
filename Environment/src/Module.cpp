#include "Module.h"

Module::Module( LoggerPtr                    setLogger,
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
std::string Module::getShortTitle() const{
    return moduleInfo.shortTitle;
}
const ModuleInfo& Module::getInfo() const{
    return moduleInfo;
}
std::vector<Command>& Module::getCommands(){
    return moduleInfo.commands;
}




bool Module::load(const EnvironmentInfo& whatToPass){
    if(!loadPtr(whatToPass, moduleInfo))
        return false;

    if(moduleInfo.title.empty() || moduleInfo.shortTitle.empty())
        return false;

    return true;
}
void Module::cleanup(){
    if(cleanupPtr)
        cleanupPtr();
}
