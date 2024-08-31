#include <LAS/LASModuleAPI.h>

using namespace LAS;
using namespace LAS::Information;
using namespace LAS::Windowing;
using namespace LAS::Logging;

LoggerPtr logger {};

const std::string TITLE                 { "Normal Window" };
const std::string LOG_TEXT              { "Logging from module" };
const std::string COMMAND_GROUP         { "normal-group" };
const std::string COMMAND_KEY           { "command" };
const std::string COMMAND_DESCRIPTION   { "description" };
const std::string COMMAND_MSG           { "This is what is displayed after command executes.\nCommand executed from [" + TITLE + "].\n" };
const Version     SDK_VERSION           { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch() };


class TestWindow : public Window{
public:
    TestWindow() : Window(TITLE){

    }
    ~TestWindow(){

    }

    void draw() override{
        ImGui::Begin(title.c_str(), &shown);

        ImGui::Text("Text here");

        if(ImGui::Button( "Log" ))
            logger->log(LOG_TEXT, Tags{TITLE});

        ImGui::End();
    }
};


TestWindow window{ };

class ModuleTestCommand : public Command {
public:
    ModuleTestCommand() : Command{COMMAND_KEY, COMMAND_DESCRIPTION} { }
    ~ModuleTestCommand() {}

    std::pair<int, std::ostringstream> execute (const std::vector<std::string>& args) override{
        std::pair<int, std::ostringstream> retVal;
        retVal.first = 0;
        retVal.second << COMMAND_MSG;
        return retVal;
    }
};

bool LASM_loadModuleInfo(ModuleInfo& moduleInfo){

    moduleInfo.sdkVersion           = SDK_VERSION;
    moduleInfo.title                = TITLE;
    moduleInfo.commandGroupName     = COMMAND_GROUP;
    moduleInfo.window               = std::make_shared<TestWindow>(window);

    // Adding commands
    moduleInfo.commands.push_back(std::make_unique<ModuleTestCommand>());          

    return true;
}
 bool LASM_init    (const EnvironmentInfo& envInfo){
    logger = envInfo.logger;

    std::cout << "From [" << TITLE << "]:\n";
    std::cout << "\tDirectory: " << envInfo.directory << "\n";
    std::cout << "\tRC File Path: " << envInfo.rcFilePath << "\n";

    ImGui::SetCurrentContext(&envInfo.context);

    return true;
 }

void LASM_cleanup(){
    std::cout << "Cleanup from [" << TITLE << "]\n";
    return;
}