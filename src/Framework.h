#include "ModuleManager.h"
#include "Logging.h"
#include "Display.h"

class Framework {
public:
    Framework (ModuleManagerPtr setModuleManager, LoggerPtr setLogger, DisplayManagerPtr setDisplayManager);
    ~Framework ();

    bool setup();

private:
    ModuleManagerPtr moduleManager;
    LoggerPtr logger;
    DisplayManagerPtr displayManager;


    bool setupDisplay();
};