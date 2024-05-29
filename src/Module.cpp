#include "Module.h"

Module::Module(LoggerPtr setLogger, loadFunction setLoad, voidNoParams setRun, voidNoParams setCleanup) :
        logger      {setLogger},
        loadPtr     {setLoad},
        runPtr      {setRun},
        cleanupPtr  {setCleanup}
{

}
Module::~Module(){

}

std::string Module::getTitle() const{
    return title;
}
std::string Module::getShortTitle() const{
    return shortTitle;
}
bool Module::getShown() const{
    return shown;
}
const ModuleSettings& Module::getSettings() const{
    return settings;
}

void Module::setShown(bool setShown) {
    shown = setShown;
}

bool Module::load(const ModuleSettings& settings){
    if(!loadPtr)
        return false;
    
    // Call the module's LASM_load(), and return false if something failed on their side
    ModuleInfo info{"New Module", "Null"};

    if(!loadPtr(settings, info))
        return false;

    title = info.title;
    shortTitle = info.shortTitle;

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
