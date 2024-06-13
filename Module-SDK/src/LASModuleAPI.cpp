#include "LASModuleAPI.h"


LAS::TestWindow window { };



bool LASM_load(UseForSetup given, ModuleInfo& moduleInfo){
    moduleInfo.title        = "New Testing";
    moduleInfo.shortTitle   = "short testing";
    moduleInfo.window       = std::make_shared<LAS::TestWindow>(window);          

    std::cout << "From NEW Testing - WORKING\n";
    std::cout << "Directory: " << given.directory << "\n";

    ImGui::SetCurrentContext(&given.context);

    return true;
}
void LASM_cleanup(){
    std::cout << "From TestModule - In Cleanup\n";
    return;
}