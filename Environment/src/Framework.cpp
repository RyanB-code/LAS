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

    if(!LAS::FrameworkSetup::setupFilesystem(filePaths))
        return false;

    if(!shell){
        if(!setupShell())
            return false;
    }

    if(!logger){
        if(!setupLogger())
            return false;
    }

    // --------------------------------------------------
    // After logging is setup, use logger for messages
    // --------------------------------------------------

    if(!moduleManager){
        if(!setupModuleManager())
            return false;
    }
    
    if(!displayManager){
        if(!setupDisplay())
            return false;
    }
    if(!setupInternalWindows())
        return false;

    // Handle commands
    if(shell)
        shell->handleCommandQueue();
    else{
        logger->log("There is no target shell for LAS", Tags{"ERROR", "SHELL"});
        return false;
    }

    if(!loadModules(filePaths.moduleDir))
        return false;

    // Non fatal if these fail
    loadAllModuleCommands();
    loadModuleWindows();

    setupCommands();

    if(!shell->readRCFile(filePaths.settingsPath))
        return false;

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
bool Framework::setupShell(){
    using namespace LAS;
    ShellPtr lasShell { new Shell{} };
    shell = lasShell;
    return true;
}
// MARK: PRIVATE FUNCTIONS
void Framework::setupCommands(){
    using namespace LAS::Commands;

    // Instantiate commands
    std::unique_ptr<TestCommand>    testCommand     {std::make_unique<TestCommand>()};
    std::unique_ptr<Set>            set             {std::make_unique<Set>(displayManager, moduleManager, logger)};
    std::unique_ptr<Manual>         manual          {std::make_unique<Manual>(shell)};



    // Add to known commands
    if(!shell->addCommand(std::move(testCommand))){
        std::ostringstream msg;
        msg << "Command [" << testCommand->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(std::move(set))){
        std::ostringstream msg;
        msg << "Command [" << set->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
    if(!shell->addCommand(std::move(manual))){
        std::ostringstream msg;
        msg << "Command [" << manual->getKey() << "] could not be added.\n";
        logger->log(msg.str(), Tags{"Shell"});
    }
}
bool Framework::setupLogger(){
    LogSettingsPtr  logSettings {new LogSettings{}};                            // Uses default settings values
    logger = std::make_shared<Logger>(Logger{logSettings});                     // Sets local logger member variable

    LogToFile logToFile{};
    if(!logToFile.setPath(LAS::FrameworkSetup::createLogFile(filePaths.logDir))){
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
bool Framework::setupModuleManager(){
    moduleManager = std::make_shared<ModuleManager>( ModuleManager{logger});
    logger->log("Module Manager setup successful", Tags{"OK"});
    return true;
}
bool Framework::setupDisplay(){
    displayManager = std::make_shared<DisplayManager>(logger);

    if(!displayManager->init()){
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

bool Framework::loadModules(const std::string& modulesDirectory) {

    if(!ImGui::GetCurrentContext()){
        logger->log("No ImGuiContext found", Tags{"ERROR", "Framework"});
        return false;
    }

    try{
        StringVector modulesThatFailedToLoad {moduleManager->loadModules(modulesDirectory, *ImGui::GetCurrentContext())}; 

        if(modulesThatFailedToLoad.size() > 0){
             // This is just for logging what failed to load
            std::ostringstream msg;
            msg << "There were [" << modulesThatFailedToLoad.size() << "] Modules that could not be loaded: ";
            for(const auto& s : modulesThatFailedToLoad){
                msg << "[" << s << "] ";
            }
            logger->log(msg.str(), Tags{"Non Fatal", "Module Manager"});
        }
        else
            logger->log("All Modules loaded successfully", Tags{"OK"});

        return true;
    }
    catch(std::filesystem::filesystem_error& e){
        logger->log("Could not find module directory [" + std::string{e.path1()} + "] to load Modules", Tags{"ERROR", "Module Manager", "Filesystem Error"});
        return false;
    }
}
void Framework::loadAllModuleCommands(){  
     // Load Commands
    StringVector commandsNotLoaded;
    StringVector moduleNames {moduleManager->getModuleNames()};
    for(auto name : moduleNames){
        commandsNotLoaded = loadModuleCommands(name);
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
StringVector Framework::loadModuleCommands(const std::string& moduleName) {
    Module& module {*moduleManager->getModule(moduleName)};

    StringVector commandsNotLoaded;

    if(module.getCommands().empty())
        return commandsNotLoaded;

    for(auto& command : module.getCommands()){
        if(!shell->addCommand(std::move(command))){
            commandsNotLoaded.push_back(command->getKey());
        }
    }

    return commandsNotLoaded;
}
void Framework::loadModuleWindows(){
    int couldntLoad {0};
    
    for(auto window : moduleManager->getAllWindows()){
        if(!displayManager->addWindow(window))
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



// MARK: LAS::FrameworkSetup Namespace
namespace LAS::FrameworkSetup{
    std::string createLogFile(const std::string& parentDir){
        std::chrono::zoned_time now { std::chrono::current_zone(), std::chrono::system_clock::now() };

        using namespace LAS::TextManip::Logging;
        std::string fileName {parentDir + std::format("{:%F} {}", now, printTime(now))};  // Outputs in format HH:MM:SS DD MM YYYY as 19:56::23 28 05 2024

        if(!ensureFile(fileName))
            return "";
        else
            return fileName;
    }
    std::string getSettingsPath(){
            // If linux, check the home directory
        #ifdef __linux__

            #ifdef DEBUG
                std::string directory {"/mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/Environment/bin/"};
                return directory + ".las-rc";
            #endif
            #ifndef DEBUG
                std::string home {getenv("HOME")};
                return home + "/.las-rc";
            #endif
        #endif

        // If windows, right now say its not supported
        #ifdef _Win32
            throw std::domain_error{"Windows is not currently supported.\nApplication aborted.\n"};
        #endif
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
    bool setupFilesystem(FilePaths& filePaths){
        // Ensure filesystem is in place
        filePaths.parentDir     = LAS::FrameworkSetup::getExeParentDir();
        filePaths.logDir        = filePaths.parentDir + "logs/";
        filePaths.moduleDir     = filePaths.parentDir + "modules/";

        // Get the correct directory for the settings file
        try{
            filePaths.settingsPath = LAS::FrameworkSetup::getSettingsPath();
        }
        catch(std::exception& e){
            std::cerr << e.what() << "\n";
            return false;
        }

        if(!LAS::ensureDirectory(filePaths.logDir)){
            std::cerr << "Error finding or creating [" << filePaths.logDir << "]";
            return false;
        }
        if(!LAS::ensureDirectory(filePaths.moduleDir)){
            std::cerr << "Error finding or creating [" << filePaths.moduleDir << "]";
            return false;
        }

        return true;
    }

}