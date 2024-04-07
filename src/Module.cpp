#include "Module.h"

Module::Module(std::string setTitle, std::string setShortTitle, LoggerPtr setLogger, Settings setSettings) :
        title       {setTitle},
        shortTitle  {setShortTitle},
        logger      {setLogger},
        settings    {setSettings}
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
const Settings& Module::getSettings() const{
    return settings;
}

void Module::setShown(bool setShown) {
    shown = setShown;
}
