#include "Framework.h"

#include <LAS/LASModuleAPI.h>

static ShooterCentral::Framework application {};


bool LASM_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    using namespace LAS;
    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};

    moduleInfo.sdkVersion       = sdkVersion;
    moduleInfo.title            = application.getWindow()->getTitle();
    moduleInfo.commandGroupName = application.getCommandGroupName();
    moduleInfo.window           = application.getWindow();

    // Add commands if needed

    return true;
}
bool LASM_init(const LAS::Information::EnvironmentInfo& envInfo){
    application.setLogger(envInfo.logger);

    application.getLogger()->log("Test log from lasm_init()", LAS::Logging::Tags{"SC"});

    return true;
}
void LASM_cleanup(){
    application.getLogger()->log("Cleanup from lasm_cleanup()", LAS::Logging::Tags{"SC"});
}