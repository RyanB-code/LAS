#include <LASModuleAPI.h>

LoggerPtr logger {};

class TestWindow : public LAS::Window{
public:
    TestWindow() : LAS::Window("Testing Window"){

    }
    ~TestWindow(){

    }

    void draw() override{
        ImGui::Begin(title.c_str(), &shown);

        ImGui::Text("New Module Helper");

        if(ImGui::Button( "Log" ))
            logger->log("From Module", Tags{"Module"});

        ImGui::End();
    }
};


TestWindow window{ };

class ModuleTestCommand : public Command {
public:
    ModuleTestCommand() : Command{"ModuleTest", "For Testing"} { }
    ~ModuleTestCommand() {}

    std::pair<int, std::ostringstream> execute (const std::vector<std::string>& args) override{
        std::pair<int, std::ostringstream> retVal;
        retVal.first = 0;
        retVal.second << "Executed from module Testing 2\n";
        return retVal;
    }
};

bool LASM_loadModuleInfo(ModuleInfo& moduleInfo){
    moduleInfo.title                = "Testing 2";
    moduleInfo.commandGroupName     = "test2";
    moduleInfo.window               = std::make_shared<TestWindow>(window);

    // Adding commands
    moduleInfo.commands.push_back(std::make_unique<ModuleTestCommand>());          

    return true;
}
 bool LASM_loadEnvironmentInfo    (const EnvironmentInfo& envInfo){
    logger = envInfo.logger;

    std::cout << "Directory: " << envInfo.directory << "\n";
    ImGui::SetCurrentContext(&envInfo.context);

    return true;
 }

void LASM_cleanup(){
    std::cout << "From TestModule - In Cleanup\n";
    return;
}