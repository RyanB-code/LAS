#include "Module.h"

Module::Module( const Logger&               setLogger,
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



bool Module::load(const GiveToModule& whatToPass){
    if(!loadPtr)
        return false;

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
