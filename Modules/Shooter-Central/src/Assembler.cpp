#include "Framework.h"

#include <LAS/LASModuleAPI.h>



bool lasm_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    using namespace LAS;
    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};


}
bool lasm_init(LAS::Information::EnvironmentInfo& envInfo){

}
void lasm_cleanup(){

}