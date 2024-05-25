#include "Framework.h"

Framework::Framework (ModuleManagerPtr setModuleManager, LoggerPtr setLogger, DisplayManagerPtr setDisplayManager) :
    moduleManager   { setModuleManager },
    logger          { setLogger },
    displayManager  { setDisplayManager }
{

}
Framework::~Framework(){

}
bool Framework::setup(){
    if(!setupDisplay())
        return false;

    return true;
}
bool Framework::setupDisplay(){
    return false;
}