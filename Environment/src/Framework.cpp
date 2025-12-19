#include "Framework.h"

using namespace LAS;

Framework::Framework () {

}
Framework::~Framework(){

}

// MARK: PUBLIC FUNCTIONS
bool Framework::setup(){

    moduleManager   = std::make_shared<ModuleManager>( );
    displayManager  = std::make_shared<DisplayManager>( );
    shell           = std::make_shared<Shell>( );


    auto [setupLoggersSuccess, consoleLogID] {setupLoggers()};

    if(!setupLoggersSuccess)
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
    log_info("Setup file logging");

    if(!setupShell(filePaths.rcPath, filePaths.commandHistoryPath))
        return false;

    if(!setupDisplay(filePaths.imGuiIniPath))
        return false;


    setupInternalWindows();

    setupCommands(); // Sets up internal commands so only needs DM, MM, and Shell to work
                                            
    if(!setupModuleManager(filePaths.moduleLibDir, filePaths.moduleFilesDir))
        return false;

    if(!loadAllModules())
        return false;

    // Non fatal if these fail
    loadAllModuleCommands();
    setupAllModules();


    if(!shell->readRCFile(filePaths.rcPath)){
        if(!ShellHelper::defaultInitializeRCFile(filePaths.rcPath)){
            log_fatal("Could not find or create new RC file at [" + filePaths.rcPath + "]");
            return false;
        }
    }
    
    readAllModuleRCFiles();

    // Handle RC file commands
    shell->handleCommandQueue(false);

    // After all is done, mark as complete
    setupComplete = true;

    // Log version information
    log_info("LAS Environment version " + LAS::Environment::getVersion());
    log_info("LAS SDK version " + LAS::SDK::getVersion());

    return true;
}
void Framework::run(){
    using namespace std::chrono;

    if(!setupComplete){
        std::cerr << "Application must be setup before running\n";
        return;
    }

    steady_clock::time_point START { steady_clock::now() };

    steady_clock::time_point currentTime    { steady_clock::now() };
    steady_clock::time_point lastTime       { currentTime };
    milliseconds updateAccumulator          { milliseconds{0} };
    milliseconds frameAccumulator           { milliseconds{0} };

    bool exitCalled { false };
    while(!exitCalled){       
        Logging::setModuleTag(COMMAND_GROUP_NAME);

        // Setup timing
        currentTime = steady_clock::now();
        auto delta { currentTime - lastTime };
        lastTime = currentTime;
        
        updateAccumulator += duration_cast<milliseconds>(delta);
        frameAccumulator += duration_cast<milliseconds>(delta);
        

        // Call backend update functions
        if(updateAccumulator >= UPDATE_INTERVAL){
            updateAccumulator -= UPDATE_INTERVAL;

            shell->handleCommandQueue();

            // Call every Module's update function they provided
            // Use modifiedEnd() to avoid unnecessary iterations to empty function objects with std::array
            for(const auto& [title, items] : moduleManager->getUpdateList() ){
                try{
                    if(items.function){
                        Logging::setModuleTag(items.tag);
                        items.function();
                    }
                }
                catch(std::exception& e){
                    log_fatal(std::format("Unhandled exception from Module '{}' caught in LAS. What: {}", title, e.what() ));  
                    Logging::setModuleTag(COMMAND_GROUP_NAME);
                    removeModule(title); 
                }
            }
            Logging::setModuleTag(COMMAND_GROUP_NAME);
        }


        // New frame rendering
        if(frameAccumulator >= FRAME_INTERVAL){
            frameAccumulator -= FRAME_INTERVAL;

            try{
                exitCalled = displayManager->refresh(moduleManager->getDrawList()); // If returns true, that means an glfwShouldWindowClose() was called
            }
            catch(const std::string& moduleTitle){
                removeModule(moduleTitle);
            }
        }

        // Wakeup thread every X ms
        std::this_thread::sleep_for(SLEEP_INTERVAL); 
    }

    return;
}
std::pair<bool, int> Framework::setupLoggers() {
    using namespace LAS::Logging;

    LogToConsole basicLogger { };
    if(!addOutput(std::make_shared<LogToConsole>(basicLogger))){
        std::cerr << "Cannot setup basic logging.\n";
        return std::pair(false, -1);
    }

    logWindow = std::make_shared<LAS::Display::LogWindow>( );
    if(!addOutput(logWindow)){
        std::cerr << "Cannot setup console logging.\n";
        return std::pair(false, -1);
    }

    setModuleTag(COMMAND_GROUP_NAME);

    return std::pair(true, basicLogger.getID());
}
bool Framework::setupShell(const std::string& rcPath, const std::string& commandHistoryPath){
    if(!shell->setRCPath(rcPath)){
        log_fatal("Failed to set shell RC file to [" + rcPath + "]");
        return false;
    }
    if(!shell->setCommandHistoryPath(commandHistoryPath)){
        log_fatal("Failed to set shell command history file to [" + commandHistoryPath + "]");
        return false;
    }

    // Add commands to command history
    StringVector historyCache { };
    ShellHelper::retrieveLines(shell->getCommandHistoryPath(), historyCache, NUM_CACHED_COMMANDS);

    for(const auto& commandText : historyCache){
        shell->addToCommandHistory(commandText);
    }

    // Make las command group
    if(!shell->addCommandGroup(COMMAND_GROUP_NAME)){
        log_fatal("Failed to create command group name [" + std::string{COMMAND_GROUP_NAME} + "]");
        return false;
    }

    return true;
}
bool Framework::setupModuleManager(const std::string& moduleLoadDir, const std::string& moduleFilesDir){
    if(!moduleManager->setModuleLoadDirectory(moduleLoadDir)){
        log_fatal("Failed to set module load directory");
        return false;
    }
    if(!moduleManager->setModuleFilesDirectory(moduleFilesDir)){
        log_fatal("Failed to set module files directory");
        return false;
    }

    log_info("Module Manager setup successful");
    return true;
}
bool Framework::setupDisplay(const std::string& imGuiIniPath){
    if(!displayManager->init(imGuiIniPath)){
        log_fatal("Error setting up necessary display libraries");
        return false;
    }

    log_info("Display setup successful");

    return true;
}
void Framework::setupInternalWindows(){ 
    using namespace LAS::Display;

    displayManager->addInternalWindow(
            LOG_WINDOW_NAME, 
            COMMAND_GROUP_NAME, 
            std::bind(&LogWindow::draw, &*logWindow) 
        );
    displayManager->addInternalWindow(
            SHELL_WINDOW_NAME, 
            COMMAND_GROUP_NAME, 
            std::bind(&Shell::draw, shell) 
        );
}
void Framework::setupCommands(){
    using namespace LAS::Commands;

    // Instantiate commands
    Manual          manual      {shell};
    ModuleControl   modulectl   {displayManager, moduleManager, shell };

    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<Manual>(manual))){
        std::ostringstream msg;
        msg << "Command [" << manual.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<ModuleControl>(modulectl))){
        std::ostringstream msg;
        msg << "Command [" << modulectl.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    /*
    Set                         set         {displayManager, moduleManager, shell};
    Print                       print       {displayManager, moduleManager};
    Echo                        echo        { };
    LAS::Commands::Information  info        { };
    Reload                      reload      {shell};
    DisplayControl              displayctl  {displayManager};


    // Add to known commands
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<Set>(set))){
        std::ostringstream msg;
        msg << "Command [" << set.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<Print>(print))){
        std::ostringstream msg;
        msg << "Command [" << print.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<Echo>(echo))){
        std::ostringstream msg;
        msg << "Command [" << echo.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<LAS::Commands::Information>(info))){
                std::ostringstream msg;
        msg << "Command [" << info.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<Reload>(reload))){
        std::ostringstream msg;
        msg << "Command [" << reload.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    if(!shell->addCommand(COMMAND_GROUP_NAME, std::make_shared<DisplayControl>(displayctl))){
        std::ostringstream msg;
        msg << "Command [" << displayctl.getKey() << "] could not be added.\n";
        log_error(msg.str());
    }
    */
}
bool Framework::loadAllModules() {
    try{
        moduleManager->loadAllModules(); 
    }
    catch(std::filesystem::filesystem_error& e){
        log_fatal("Could not find module directory [" + std::string{e.path1()} + "] to load Modules");
        return false;
    }

    return true;
}
void Framework::loadAllModuleCommands(){  
    int couldntLoad { 0 };

    for(const auto& [title, modulePtr] : moduleManager->getModuleList() ){
        Module& module { *modulePtr };
        const ModuleInfo& info { module.getModuleInfo() };

        if(!shell->addCommandGroup(info.shortTag)){
            moduleManager->removeModule(title);
            log_error("Failed to load commands for Module [" + info.title +"]. Module removed.");
            ++couldntLoad;
            continue;
        }   

        if(module.getCommands().empty())
            continue;

        // Load the commands
        int commandsNotLoaded { 0 };
        for(auto& command : module.getCommands()){
            if(!shell->addCommand(info.shortTag, command))
                ++commandsNotLoaded;
        }

        if(commandsNotLoaded > 0)
            log_warn(std::format("There were [{}] commands not loaded from Module [{}]", couldntLoad, info.title));
    }

    if(couldntLoad > 0)
        log_warn(std::format("There were [{}] Modules that could not load commands", couldntLoad));
    else
        log_info("All commands successfully loaded from all modules");
}
void Framework::setupAllModules() {
    log_info("Setting up all Modules...");
    moduleManager->setupAllModules( *ImGui::GetCurrentContext() );
}
void Framework::removeModule(const std::string& title){
    log_warn(std::format("Shutting down Module '{}'...", title));

    shell->removeCommandGroup(moduleManager->getModule(title).getModuleInfo().shortTag);
    moduleManager->removeModule(title);

    log_warn("Module shutdown complete");

}
void Framework::readAllModuleRCFiles(){
    for(const auto& [title, modulePtr] : moduleManager->getModuleList() )
        readModuleRCFile(modulePtr->getRCFilePath());
}
bool Framework::readModuleRCFile (const std::string& rcFilePath){
    std::queue<std::string> commandLines;

    if(LAS::ShellHelper::readRCFile(rcFilePath, commandLines)){
        for (/*Nothing*/; !commandLines.empty(); commandLines.pop()){
            shell->addToQueue(commandLines.front());
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


bool setupFileLogger(const std::string& logDir){

    LogToFile logToFile{};
    if(!logToFile.setPath(LAS::FrameworkSetup::createLogFile(logDir))){
         log_fatal("Could not create a log file for the current instance");
         return false;
    }
    
    // If all is good, add LogToFile to logger
    if(!Logging::addOutput(std::make_shared<LogToFile>(logToFile))){
        log_fatal("Could not add a file logger as a log output");
        return false;
    }


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
                log_fatal("Only Linux is currently supported.\n");
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
                log_fatal("Error finding or creating [" + filePaths.logDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleLibDir)){
                log_fatal("Error finding or creating [" + filePaths.moduleLibDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleFilesDir)){
                log_fatal("Error finding or creating [" + filePaths.moduleFilesDir + "]");
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.dotLASDir)){
                log_fatal("Error finding or creating [" + filePaths.dotLASDir + "]");
                return false;
            }
        }
        catch(std::filesystem::filesystem_error& e){
            log_fatal(std::format("Fatal error in LAS filesystem setup. What: [{}]. File: [{}]", e.what(), e.path1().string() ));
            return false;
        }

        return true;
    }
}
