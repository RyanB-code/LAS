#include "Framework.h"

using namespace LAS;

Framework::Framework (  LoggerPtr           setLogger,
                        ModuleManagerPtr    setModuleManager,
                        DisplayManagerPtr   setDisplayManager,
                        ShellPtr            setShell
                    )
                    :
                        logger          {setLogger},
                        moduleManager   {setModuleManager},
                        displayManager  {setDisplayManager},
                        shell           {setShell}
{

}
Framework::~Framework(){

}

// MARK: PUBLIC FUNCTIONS
bool Framework::setup(){

    FilePaths filePaths;

    if(!LAS::FrameworkSetup::setupFilesystem(filePaths))
        return false;

    if(!shell){
        if(!setupShell(filePaths.rcPath, filePaths.commandHistoryPath))
            return false;
    }

    if(!logger){
        if(!setupLogger(filePaths.logDir))
            return false;
    }

    // --------------------------------------------------
    // After logging is setup, use logger for messages
    // --------------------------------------------------
    
    if(!displayManager){
        if(!setupDisplay(filePaths.imGuiIniPath))
            return false;
    }
    if(!setupInternalWindows())
        return false;

    // --------------------------------------------------
    // Up until setupInternalWindows() is called, all logs
    // are displayed in the log file only since no ImGui
    // window is present to accept and display logs
    // --------------------------------------------------    

    if(!moduleManager){
        if(!setupModuleManager(filePaths.moduleLibDir, filePaths.moduleFilesDir))
            return false;
    }

    if(!loadAllModules(filePaths.moduleLibDir, filePaths.moduleFilesDir))
        return false;

    // Non fatal if these fail
    loadAllModuleCommands();
    loadAllModuleWindows();
    setupCommands();

    
    if(!shell->readRCFile(filePaths.rcPath)){
        if(!ShellHelper::defaultInitializeRCFile(filePaths.rcPath)){
            logger->log("Could not find or create new RC file at [" + filePaths.rcPath + "]", Tags{"ERROR", "SHELL"});
            return false;
        }
    }
    
    readAllModuleRCFiles();

    // Handle RC file commands
    if(shell)
        shell->handleCommandQueue(false);
    else{
        logger->log("There is no target shell for LAS", Tags{"ERROR", "SHELL"});
        return false;
    }

    // After all is done, mark as complete
    setupComplete = true;
    return true;
}
void Framework::run(){
    if(!setupComplete){
        std::cerr << "Application must be setup before running\n";
        return;
    }

    // If refresh() returns true, that means an glfwShouldWindowClose() was called
    while(!displayManager->refresh()){       
        shell->handleCommandQueue();
    }

    return;
}

// MARK: PRIVATE FUNCTIONS
// MARK: Setup Functions
bool Framework::setupShell(const std::string& rcPath, const std::string& commandHistoryPath){
    using namespace LAS;
    ShellPtr lasShell { new Shell{} };
    shell = lasShell;

    if(!shell->setRCPath(rcPath)){
        std::cerr << "Failed to set shell RC file to \"" << rcPath << "\"\n";
        return false;
    }
    if(!shell->setCommandHistoryPath(commandHistoryPath)){
        std::cerr << "Failed to set shell command history file\n";
        return false;
    }
    // Add commands to command history
    StringVector historyCache { };
    ShellHelper::retrieveLines(shell->getCommandHistoryPath(), historyCache, cacheLastNumberOfCommands);
    for(const auto& commandText : historyCache){
        shell->getWindow()->addToCommandHistory(commandText);
    }

    // Make las command group
    if(!shell->addCommandGroup(commandGroupName)){
        std::cerr << "Failed to create \"" << commandGroupName << "\" command group.\n";
        return false;
    }

    return true;
}
// MARK:: Add Commands
void Framework::setupCommands(){
    using namespace LAS::Commands;

    // Instantiate commands
    std::unique_ptr<Set>                            set         {std::make_unique<Set>                          (displayManager, moduleManager, logger, shell)};
    std::unique_ptr<Manual>                         manual      {std::make_unique<Manual>                       (shell) };
    std::unique_ptr<Print>                          print       {std::make_unique<Print>                        (displayManager, moduleManager, logger)};
    std::unique_ptr<Echo>                           echo        {std::make_unique<Echo>                         () };
    std::unique_ptr<ModuleControl>                  modulectl   {std::make_unique<ModuleControl>                (displayManager, moduleManager, shell) };
    std::unique_ptr<LAS::Commands::Information>     info        {std::make_unique<LAS::Commands::Information>   () };
    std::unique_ptr<Reload>                         reload      {std::make_unique<Reload>                       (shell) };
    std::unique_ptr<DisplayControl>                 displayctl  {std::make_unique<DisplayControl>               (displayManager) };


    // Add to known commands
    if(!shell->addCommand(commandGroupName, std::move(set))){
        std::ostringstream msg;
        msg << "Command [" << set->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(manual))){
        std::ostringstream msg;
        msg << "Command [" << manual->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(print))){
        std::ostringstream msg;
        msg << "Command [" << print->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(echo))){
        std::ostringstream msg;
        msg << "Command [" << echo->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(modulectl))){
        std::ostringstream msg;
        msg << "Command [" << modulectl->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(info))){
        std::ostringstream msg;
        msg << "Command [" << info->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(reload))){
        std::ostringstream msg;
        msg << "Command [" << reload->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(commandGroupName, std::move(displayctl))){
        std::ostringstream msg;
        msg << "Command [" << displayctl->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
}
bool Framework::setupLogger(const std::string& logDir){
    LogSettingsPtr  logSettings {new LogSettings{}};                            // Uses default settings values
    logger = std::make_shared<Logger>(Logger{logSettings});                     // Sets local logger member variable

    LogToFile logToFile{};
    if(!logToFile.setPath(LAS::FrameworkSetup::createLogFile(logDir))){
         std::cerr << "Could not create a log file for the current instance\n";
         return false;
    }
    
    // If all is good, add LogToFile to logger
    if(!logger->addOutput(std::make_shared<LogToFile>(logToFile))){
        std::cerr << "Could not add default LogToFile as an output for the logger\n";
        return false;
    }

    logger->log("Logger setup successful", Tags{"OK"});
    return true;
}
bool Framework::setupModuleManager(const std::string& moduleLoadDir, const std::string& moduleFilesDir){
    moduleManager = std::make_shared<ModuleManager>( ModuleManager{logger});

    if(!moduleManager->setModuleLoadDirectory(moduleLoadDir)){
        logger->log("Failed to set module load directory", Tags{"FATAL", "Module Manager"});
        return false;
    }
    if(!moduleManager->setModuleFilesDirectory(moduleFilesDir)){
        logger->log("Failed to set module files directory", Tags{"FATAL", "Module Manager"});
        return false;
    }

    logger->log("Module Manager setup successful", Tags{"OK"});
    return true;
}
bool Framework::setupDisplay(const std::string& imGuiIniPath){
    displayManager = std::make_shared<DisplayManager>(logger);

    if(!displayManager->init(imGuiIniPath)){
        logger->log("Error setting up necessary display libraries", Tags{"ERROR", "Display Manager"});
        return false;
    }

    logger->log("Display setup successful", Tags{"OK"});

    return true;
}
bool Framework::setupInternalWindows(){

    // Setup log viewer window
    LogWindow   logWindow{logger->getLogSettings()};
    LogToWindow logToWindow{std::make_shared<LogWindow>(logWindow)};

    logger->addOutput(std::make_shared<LogToWindow>(logToWindow));

    if(!displayManager->addWindow(logToWindow.getWindow())){
        logger->log("Log viewer could not be added to window manager", Tags{"ERROR", "Display Manager"});
        return false;
    }
    logger->log("Log viewer setup successful", Tags{"OK"});


    // Setup console window
    if(!displayManager->addWindow(shell->getWindow())){
        logger->log("Console could not be added to window manager", Tags{"ERROR", "Display Manager"});
        return false;
    }
    if(!shell->addOutput(shell->getWindow())){
        logger->log("Console Window's Shell Output could not be added to Shell Outputs", Tags{"ERROR", "Shell"});
        return false;
    }


    logger->log("Console setup successful", Tags{"OK"});

    return true;
}
// MARK: Load Functions
bool Framework::loadAllModules(const std::string& moduleLibDirectory, const std::string& moduleFilesDirectory) {
    if(!ImGui::GetCurrentContext()){
        logger->log("No ImGuiContext found", Tags{"ERROR", "Framework"});
        return false;
    }

    try{
        StringVector modulesThatFailedToLoad {};
        moduleManager->loadAllModules(*ImGui::GetCurrentContext(), modulesThatFailedToLoad, moduleLibDirectory, moduleFilesDirectory); 

        // This is just for logging what failed to load
        if(modulesThatFailedToLoad.size() > 0){
            std::ostringstream msg;
            msg << "There were [" << modulesThatFailedToLoad.size() << "] Modules that could not be loaded: ";
            for(const auto& s : modulesThatFailedToLoad){
                msg << "[" << s << "] ";
            }
            logger->log(msg.str(), Tags{"Non Fatal", "Module Manager"});
        }
        else
            logger->log("All Modules loaded successfully", Tags{"OK"});
    }
    catch(std::filesystem::filesystem_error& e){
        logger->log("Could not find module directory [" + std::string{e.path1()} + "] to load Modules", Tags{"ERROR", "Module Manager", "Filesystem Error"});
        return false;
    }

    return true;
}
void Framework::loadAllModuleWindows(){
    int couldntLoad {0};
    
    for(const auto& name : moduleManager->getModuleNames()){
        if(!loadModuleWindow(name))
            ++couldntLoad;
    }
    if(couldntLoad > 0){
        std::ostringstream msg;
        msg << "There were [" << couldntLoad << "] windows that could not be loaded from modules";
        logger->log(msg.str(), Tags{"Non Fatal", "Framework"});
    }
    else
        logger->log("All windows successfully loaded from all modules", Tags{"OK"});
}
bool Framework::loadModuleWindow(const std::string& name){
    if(!moduleManager->containsModule(name))
        return false;
    
    if(!displayManager->addWindow(moduleManager->getModule(name)->getWindow())){
        logger->log("Failed to load window for module [" + name +"]. Module ID or window name collision", Tags{"ERROR", "Display Manager", "Module Manager"});
        return false;
    }
    
    return true;
}
void Framework::loadAllModuleCommands(){  
    StringVector moduleNames {moduleManager->getModuleNames()};

    for(auto name : moduleNames){
        StringVector commandsNotLoaded {};
        loadModuleCommands(name, commandsNotLoaded); 

        // Report how many commands were not loaded
        if(!commandsNotLoaded.empty()){
            std::ostringstream msg;
            msg << "[" << commandsNotLoaded.size() << "] commands could not be loaded from Module [" << name << "]";
            logger->log(msg.str(), Tags{"Framework"});
            commandsNotLoaded.clear();
        }
        else
            logger->log("All commands loaded successfully from module [" + name + "]", Tags {"OK"});
    }
}
bool Framework::loadModuleCommands(const std::string& moduleName, StringVector& commandsNotLoaded) {
    Module& module {*moduleManager->getModule(moduleName)};

    if(!shell->addCommandGroup(module.getGroupName()))
        return false;

    if(module.getCommands().empty())
        return true;

    // Load the commands
    for(auto& command : module.getCommands()){
        if(!shell->addCommand(module.getGroupName(), std::move(command))){
            commandsNotLoaded.push_back(command->getKey());
        }
    }

    return true;
}

void Framework::readAllModuleRCFiles(){
    StringVector names {moduleManager->getModuleNames()};

    for(const auto& s : names){
        readModuleRCFile(s);
    }
}
bool Framework::readModuleRCFile (const std::string& name){
    if(!moduleManager->containsModule(name))
        return false; 

    ModulePtr moduleBuffer {moduleManager->getModule(name)};

    std::queue<std::string> commandLines;
    if(LAS::ShellHelper::readRCFile(moduleBuffer->getRCFilePath(), commandLines)){
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
    bool setupFilesystem(FilePaths& filePaths) {
        // Ensure filesystem is in place
        #ifdef DEBUG
            filePaths.parentDir             = LAS::FrameworkSetup::getExeParentDir();
        #else
            #ifdef __linux__
                std::string home {getenv("HOME")};
            #else
                std::cerr << "Only Linux is currently supported.\n";
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
                std::cerr << "Error finding or creating [" << filePaths.logDir << "]";
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleLibDir)){
                std::cerr << "Error finding or creating [" << filePaths.moduleLibDir << "]";
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.moduleFilesDir)){
                std::cerr << "Error finding or creating [" << filePaths.moduleFilesDir << "]";
                return false;
            }
            if(!LAS::ensureDirectory(filePaths.dotLASDir)){
                std::cerr << "Error finding or creating [" << filePaths.dotLASDir << "]";
                return false;
            }
        }
        catch(std::filesystem::filesystem_error& e){
            std::cerr << "Fatal error in LAS filesystem setup.\n\tWhat: " << e.what() << "\n\tFile: " << e.path1().string() << "\n";
            return false;
        }

        return true;
    }
}