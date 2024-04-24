#include "Framework.h"

Framework::Framework (ModuleManagerPtr setModuleManager, LoggerPtr setLogger, DisplayManagerPtr setDisplayManager) :
    moduleManager   { setModuleManager },
    displayManager  { setDisplayManager },
    logger          { setLogger }
{

}
Framework::~Framework(){

}