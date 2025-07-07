#include "Framework.h"

using namespace LAS;

Framework::Framework () {

}
Framework::~Framework(){

}

// MARK: PUBLIC FUNCTIONS
bool Framework::setup(){

    auto [addConsoleLogSuccess, consoleLogID] {FrameworkSetup::setupBasicLogger()};
    if(!addConsoleLogSuccess)
        return false;

    // --------------------------------------------------
    // Use global logging calls for all logging from here
    // on out
    // --------------------------------------------------

    FilePaths filePaths;
    if(!FrameworkSetup::setupFilesystem(filePaths))
        return false;

    if(!FrameworkSetup::setupFileLogger(filePaths.logDir))
        return false;

    Logging::disableOutput(consoleLogID);   // Disable console logging after file logging is established

    if(!setupShell(filePaths.rcPath, filePaths.commandHistoryPath))
        return false;
    
    if(!setupDisplay(filePaths.imGuiIniPath))
        return false;

    if(!setupInternalWindows())
        return false;

    if(!setupModuleManager(filePaths.moduleLibDir, filePaths.moduleFilesDir))
        return false;

    if(!loadAllModules(filePaths.moduleLibDir, filePaths.moduleFilesDir))
        return false;

    // Non fatal if these fail
    loadAllModuleCommands();
    loadAllModuleWindows();
    setupCommands();

    
    if(!shell.readRCFile(filePaths.rcPath)){
        if(!ShellHelper::defaultInitializeRCFile(filePaths.rcPath)){
            log_critical("Could not find or create new RC file at [" + filePaths.rcPath + "]");
            return false;
        }
    }
    
    readAllModuleRCFiles();

    // Handle RC file commands
    shell.handleCommandQueue(false);

    // After all is done, mark as complete
    setupComplete = true;

    // Log version information
    log_info("LAS Environment version " + LAS::Environment::getVersion());
    log_info("LAS SDK version " + LAS::SDK::getVersion());

    log_info("Logger setup successful");

    return true;
}
void Framework::run(){
    if(!setupComplete){
        std::cerr << "Application must be setup before running\n";
        return;
    }

    // 1. Call update on internal controllers
    // 2. Update all external Module windows
    

    // If refresh() returns true, that means an glfwShouldWindowClose() was called
    while(!displayManager.refresh()){       
        shell.handleCommandQueue();
    }

    return;
}

// MARK: PRIVATE FUNCTIONS
// MARK: Setup Functions
bool Framework::setupShell(const std::string& rcPath, const std::string& commandHistoryPath){
    if(!shell.setRCPath(rcPath)){
        log_critical("Failed to set shell RC file to [" + rcPath + "]");
        return false;
    }
    if(!shell.setCommandHistoryPath(commandHistoryPath)){
        log_critical("Failed to set shell command history file to [" + commandHistoryPath + "]");
        return false;
    }

    // Add commands to command history
    StringVector historyCache { };
    ShellHelper::retrieveLines(shell.getCommandHistoryPath(), historyCache, NUM_CACHED_COMMANDS);

    for(const auto& commandText : historyCache){
        shell.addToCommandHistory(commandText);
    }

    // Make las command group
    if(!shell.addCommandGroup(COMMAND_GROUP_NAME)){
        log_critical("Failed to create command group name [" + std::string{COMMAND_GROUP_NAME} + "]");
        return false;
    }

    return true;
}
// MARK:: Add Commands
void Framework::setupCommands(){
    using namespace LAS::Commands;

    // Instantiate commands
    std::unique_ptr<Set>                            set         {std::make_unique<Set>                          (displayManager, moduleManager, shell)};
    std::unique_ptr<Manual>                         manual      {std::make_unique<Manual>                       (shell) };
    std::unique_ptr<Print>                          print       {std::make_unique<Print>                        (displayManager, moduleManager)};
    std::unique_ptr<Echo>                           echo        {std::make_unique<Echo>                         () };
    std::unique_ptr<ModuleControl>                  modulectl   {std::make_unique<ModuleControl>                (displayManager, moduleManager, shell) };
    std::unique_ptr<LAS::Commands::Information>     info        {std::make_unique<LAS::Commands::Information>   () };
    std::unique_ptr<Reload>                         reload      {std::make_unique<Reload>                       (shell) };
    std::unique_ptr<DisplayControl>                 displayctl  {std::make_unique<DisplayControl>               (displayManager) };


    // Add to known commands
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(set))){
        std::ostringstream msg;
        msg << "Command [" << set->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(manual))){
        std::ostringstream msg;
        msg << "Command [" << manual->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(print))){
        std::ostringstream msg;
        msg << "Command [" << print->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(echo))){
        std::ostringstream msg;
        msg << "Command [" << echo->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(modulectl))){
        std::ostringstream msg;
        msg << "Command [" << modulectl->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(info))){
        std::ostringstream msg;
        msg << "Command [" << info->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(reload))){
        std::ostringstream msg;
        msg << "Command [" << reload->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell.addCommand(COMMAND_GROUP_NAME, std::move(displayctl))){
        std::ostringstream msg;
        msg << "Command [" << displayctl->getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
}

bool Framework::setupModuleManager(const std::string& moduleLoadDir, const std::string& moduleFilesDir){
    if(!moduleManager.setModuleLoadDirectory(moduleLoadDir)){
        log_critical("Failed to set module load directory");
        return false;
    }
    if(!moduleManager.setModuleFilesDirectory(moduleFilesDir)){
        log_critical("Failed to set module files directory");
        return false;
    }

    log_info("Module Manager setup successful");
    return true;
}
bool Framework::setupDisplay(const std::string& imGuiIniPath){
    if(!displayManager.init(imGuiIniPath)){
        log_critical("Error setting up necessary display libraries");
        return false;
    }

    log_info("Display setup successful");

    return true;
}
bool Framework::setupInternalWindows(){
    using namespace Display;
    using namespace ModuleFunctions;

    Logging::addOutput(std::make_shared<LogWindow>(logWindow));

    std::function<void()> drawFunction { std::bind(&LogWindow::draw, &logWindow) };

    if(!displayManager.addWindow(LOG_WINDOW_NAME, drawFunction)){
        log_error("Log viewer could not be added to window manager");
        return false;
    }
    logWindow.setShown(displayManager.shown(LOG_WINDOW_NAME));
    log_info("Log viewer setup successful");

    // Setup console window
    if(!displayManager.addWindow(SHELL_WINDOW_NAME, shell.draw)){
        log_error("Console could not be added to window manager");
        return false;
    }
    log_info("Console setup successful");

    return true;
}
// MARK: Load Functions
bool Framework::loadAllModules(const std::string& moduleLibDirectory, const std::string& moduleFilesDirectory) {
    if(!ImGui::GetCurrentContext()){
        log_critical("No ImGuiContext found");
        return false;
    }

    try{
        StringVector modulesThatFailedToLoad {};
        moduleManager.loadAllModules(*ImGui::GetCurrentContext(), modulesThatFailedToLoad, moduleLibDirectory, moduleFilesDirectory); 

        // This is just for logging what failed to load
        if(modulesThatFailedToLoad.size() > 0){
            std::ostringstream msg;
            msg << "There were [" << modulesThatFailedToLoad.size() << "] Modules that could not be loaded: ";
            for(const auto& s : modulesThatFailedToLoad){
                msg << "[" << s << "] ";
            }
            log_warn(msg.str());
        }
        else
            log_info("All Modules loaded successfully");
    }
    catch(std::filesystem::filesystem_error& e){
        log_critical("Could not find module directory [" + std::string{e.path1()} + "] to load Modules");
        return false;
    }

    return true;
}
void Framework::loadAllModuleWindows(){
    int couldntLoad {0};
    
    for(const auto& name : moduleManager.getModuleNames()){
        if(!loadModuleWindow(name))
            ++couldntLoad;
    }
    if(couldntLoad > 0){
        std::ostringstream msg;
        msg << "There were [" << couldntLoad << "] windows that could not be loaded from modules";
        log_warn(msg.str());
    }
    else
        log_info("All windows successfully loaded from all modules");
}
bool Framework::loadModuleWindow(const std::string& name){
    if(!moduleManager.containsModule(name))
        return false;
    
    if(!displayManager.addWindow(moduleManager.getModule(name)->getModuleInfo.title, getModuleInfo.drawFunction)){
        log_error("Failed to load window for module [" + name +"]. Module ID or window name collision");
        return false;
    }
    
    return true;
}
void Framework::loadAllModuleCommands(){  
    StringVector moduleNames {moduleManager.getModuleNames()};

    for(auto name : moduleNames){
        StringVector commandsNotLoaded {};
        loadModuleCommands(name, commandsNotLoaded); 

        // Report how many commands were not loaded
        if(!commandsNotLoaded.empty()){
            std::ostringstream msg;
            msg << "[" << commandsNotLoaded.size() << "] commands could not be loaded from Module [" << name << "]";
            log_warn(msg.str());
            commandsNotLoaded.clear();
        }
        else
            log_info("All commands loaded successfully from module [" + name + "]");
    }
}
bool Framework::loadModuleCommands(const std::string& moduleName, StringVector& commandsNotLoaded) {
    Module& module {*moduleManager.getModule(moduleName)};

    if(!shell.addCommandGroup(module.getGroupName()))
        return false;

    if(module.getCommands().empty())
        return true;

    // Load the commands
    for(auto& command : module.getCommands()){
        if(!shell.addCommand(module.getGroupName(), std::move(command))){
            commandsNotLoaded.push_back(command->getKey());
        }
    }

    return true;
}

void Framework::readAllModuleRCFiles(){
    StringVector names {moduleManager.getModuleNames()};

    for(const auto& s : names){
        readModuleRCFile(s);
    }
}
bool Framework::readModuleRCFile (const std::string& name){
    if(!moduleManager.containsModule(name))
        return false; 

    ModulePtr moduleBuffer {moduleManager.getModule(name)};

    std::queue<std::string> commandLines;
    if(LAS::ShellHelper::readRCFile(moduleBuffer->getRCFilePath(), commandLines)){
        for (/*Nothing*/; !commandLines.empty(); commandLines.pop()){
            shell.addToQueue(commandLines.front());
        }
    }
    return true;
}



// MARK: LAS::FrameworkSetup Namespace
namespace LAS::FrameworkSetup{
    std::string createLogFile(const std::string& parentDir){
        std::chrono::zoned_time now { std::chrono::current_zone(), std::chrono::system_clock::now() };

        std::string fileName {parentDir + std::format("{:%F} {}", now, Logging::printTime(now) + ".log")};  // Outputs in format YYY-MM-DD HH:MM::SS.log

        if(!ensureFile(fileName))
            return "";
        else
            return fileName;
    }
    std::string getExeParentDir(){
        #ifdef __linux__
            std::string exePath {std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"})};
        #endif

        std::string parentDir;

        // Iterates backwards over the EXE path and just goes until it finds the first slash and then returns that string
        for(int i{static_cast<int>(exePath.length())-1}; i >= 0; --i){
            char c {exePath.at(i)};
            if(c == '/' || c == '\\'){
                parentDir = exePath.substr(0, ++i);
                break;
            }
        }
        return parentDir;
    }

std::pair<bool, int> setupBasicLogger() {
    using namespace LAS::Logging;

    LogToConsole basicLogger { };
    if(!addOutput(std::make_shared<LogToConsole>(basicLogger))){
        std::cerr << "Cannot setup basic logging.\n";
        return std::pair(false, -1);
    }

    return std::pair(true, basicLogger.getID());
}
bool setupFileLogger(const std::string& logDir){

    LogToFile logToFile{};
    if(!logToFile.setPath(LAS::FrameworkSetup::createLogFile(logDir))){
         log_critical("Could not create a log file for the current instance");
         return false;
    }
    
    // If all is good, add LogToFile to logger
    if(!Logging::addOutput(std::make_shared<LogToFile>(logToFile))){
        log_critical("Could not add a file logger as a log output");
        return false;
    }

    log_info("Setup file logging");

    return true;
}
bool setupFilesystem(FilePaths& filePaths) {
        // Ensure filesystem is in place
        #ifdef DEBUG
            filePaths.parentDir             = LAS::FrameworkSetup::getExeParentDir();
        #else
            #ifdef __linux__
                std::string home {getenv("HOME")};
            #else
                log_critical("Only Linux is currently supported.\n");
                return false;
            #endif

            filePaths.parentDir             = home + "/Documents/LAS/";
        #endif

        filePaths.dotLASDir             = filePaths.parentDir + ".las/";
        filePaths.rcPath                = filePaths.dotLASDir + ".las-rc";  // Do not create here, it is initialized in shell

        filePaths.moduleLibDir          = filePaths.parentDir + "Module Libraries/";
        filePaths.moduleFilesDir        = filePaths.parentDir + "Module Files/";

        filePaths.commandHistoryPath    = filePaths.dotLASDir + ".las-history";
        filePaths.logDir                = filePaths.dotLASDir + "logs/";

        filePaths.imGuiIniPath          = filePaths.dotLASDir + "imgui.ini";

        // Check paths are good
        try {
            if(!LAS::ensureDirectory(filePaths.logDir)){
                log_critical("Error finding or creating [" + filePaths.logDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleLibDir)){
                log_critical("Error finding or creating [" + filePaths.moduleLibDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleFilesDir)){
                log_critical("Error finding or creating [" + filePaths.moduleFilesDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.dotLASDir)){
                log_critical("Error finding or creating [" + filePaths.dotLASDir + "]");
                return false;
            }
        }
        catch(std::filesystem::filesystem_error& e){
            log_critical(std::format("Fatal error in LAS filesystem setup. What: [{}]. File: [{}]", e.what(), e.path1().string() ));
            return false;
        }

        return true;
    }
}
