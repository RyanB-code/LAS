#include "Module.h"

Module::Module( const Logger&               setLogger,
                LAS::Modules::loadFunction   setLoad, 
                LAS::Modules::voidNoParams   setRun, 
                LAS::Modules::voidNoParams   setCleanup) :
        logger      {setLogger},
        loadPtr     {setLoad},
        runPtr      {setRun},
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
bool& Module::show(){

    if(moduleInfo.shown){
        if(*moduleInfo.shown)
            run();
    }
    
    return *moduleInfo.shown;
}
const ModuleInfo& Module::getInfo() const{
    return moduleInfo;
}


bool Module::load(const PassToModule& whatToPass){
    if(!loadPtr)
        return false;

    ModuleInfo passedInfo;  // Pass this to the Module across DLL boundaries

    if(!loadPtr(whatToPass, passedInfo))
        return false;

    if(passedInfo.title.empty() || passedInfo.shortTitle.empty() || !passedInfo.shown)
        return false;

    // Assign to this module's moduleInfo
    moduleInfo.title        = passedInfo.title;
    moduleInfo.shortTitle   = passedInfo.shortTitle;
    moduleInfo.shown        = passedInfo.shown;

    return true;
}
void Module::run(){
    if(runPtr)
        runPtr();
}
void Module::cleanup(){
    if(cleanupPtr)
        cleanupPtr();
}
