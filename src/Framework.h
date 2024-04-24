#include "ModuleManager.h"
#include "Logging.h"
#include "Display.h"

class Framework {
public:
    Framework (ModuleManagerPtr setModuleManager, LoggerPtr setLogger, DisplayManagerPtr setDisplayManager);
    ~Framework ();

    

private:
    ModuleManagerPtr moduleManager;
    LoggerPtr logger;
    DisplayManagerPtr displayManager;
};