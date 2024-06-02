#include "Framework.h"

Framework::Framework (  LoggerPtr           setLogger,
                        ModuleManagerPtr    setModuleManager,
                        DisplayManagerPtr   setDisplayManager
                    )
                    :
                        logger          {setLogger},
                        moduleManager   {setModuleManager},
                        displayManager  {setDisplayManager}
{

}
Framework::~Framework(){

}

// MARK: PUBLIC FUNCTIONS
bool Framework::setup(){

    if(!LAS::FrameworkSetup::setupFilesystem(filePaths)){
        std::cerr << "Could not setup filesystem\n";
        return false;
    }

    setupCommands();

    if(!readSetupFile(filePaths.settingsPath)){
        logger->log("Error reading settings file at [" + filePaths.settingsPath + "]", Tags{"Filesystem Error"});
        return false;
    }

    // Default Setup of Logger
    if(!logger){
        LogSettingsPtr  logSettings {new LogSettings{}};                            // Uses default settings values
        LogToFile       logToFile    {};

        logger = std::make_shared<Logger>(Logger{logSettings});                     // Sets local logger member variable
        logToFile.setPath(LAS::FrameworkSetup::createLogFile(filePaths.logDir));

        // Check to ensure log file is good
        if(logToFile.getPath().empty()){
            std::cerr << "Could not create instance log file\n";
            return false;
        }

        // If all is good, add LogToFile to logger
        logger->addOutput(std::make_shared<LogToFile>(logToFile));
    }

    // --------------------------------------------------
    // After logging is setup, use logger for messages
    // --------------------------------------------------

    if(!moduleManager){
        moduleManager = std::make_shared<ModuleManager>( ModuleManager{*logger});
    }
    if(!displayManager){
        displayManager = std::make_shared<DisplayManager>(*logger.get(), moduleManager);
        if(!displayManager->init()){
            logger->log("Error setting up necessary display libraries", Tags{"Display Manager"});
            return false;
        }
    }

    handleCommandQueue();

    // After all is setup, load modules
    if(!loadModules(filePaths.moduleDir)){
        logger->log("Error loading modules.", Tags{"Module Loader"});
        return false;
    }

    
    return true;
}
void Framework::run(){

    // If refresh() returns true, that means an glfwShouldWindowClose() was called
    while(!displayManager->refresh()){

    }

    return;
}
// MARK: PRIVATE FUNCTIONS
void Framework::setupCommands(){

    // Instantiate commands
    std::unique_ptr<TestCommand> testCommand {new TestCommand()};

    // Add to known commands
    if(!addCommand(std::move(testCommand))){
        std::cerr << "Command [" + testCommand->getKey() << "] could not be added.\n";
    }
}
bool Framework::addCommand(std::unique_ptr<Command> command){
    if(commands.contains(command->getKey())){
        return false;
    }
    else{
        commands.try_emplace(command->getKey(), std::move(command));
        return true;
    }
    return false;
}
bool Framework::handleCommandQueue(){\

    // This just prints each command entry riught now
    std::cout << "From Framework::handleCommandQueue():\n";
    for (/*Nothing*/; !commandQueue.empty(); commandQueue.pop())
        std::cout << "\t" << commandQueue.front() << '\n';
    std::cout << "\n";
    /* This is for inputting quoted buffer
    while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
            arguments.push_back(buffer);                // Add to argument list
        }
    */

    return false;
}
bool Framework::loadModules(const std::string& modulesDirectory) {

    if(!ImGui::GetCurrentContext()){
        logger->log("No ImGuiContext found", Tags{"Module Loader", "ERROR"});
        return false;
    }

    try{
        std::pair<int, StringVector> modulesThatFailedToLoad {moduleManager->loadModules(modulesDirectory, *ImGui::GetCurrentContext())}; 

        // This is just for logging what failed to load
        std::ostringstream msg;
        msg << "There were [" << modulesThatFailedToLoad.first << "] Modules that could not be loaded: ";
        for(const auto& s : modulesThatFailedToLoad.second){
            msg << "[" << s << "] ";
        }
        logger->log(msg.str(), Tags{"Module Manager"});

        return true;
    }
    catch(std::filesystem::filesystem_error& e){
        logger->log("Could not find module directory [" + std::string{e.path1()} + "] to load Modules", Tags{"Module Loader", "Filesystem Error"});
        return false;
    }
}
bool Framework::readSetupFile(const std::string& path){

     if(std::filesystem::exists(path)){
        std::ifstream settingsFile {path}; 
        std::string line;

        // Read line by line
        while (std::getline(settingsFile, line))
        {
            std::stringstream inputStream{};
            inputStream << line;
            std::string buffer {inputStream.str()};

            // Filter out comments and newlines, add the rest to commandQueue
            if(!buffer.starts_with("#") && !buffer.starts_with('\n') && !buffer.empty())
                commandQueue.push(buffer);
        }
        return true;
    }
    else{
        std::ofstream newSettings {path, std::ios::trunc};

        if(!std::filesystem::exists(path)){
            std::cerr << "Could not create new configuration file at [" << path << "]\n";
            return false;
        }

        newSettings << "# Life Application Suite Configuration File\n# Any commands entered here will be executed upon each startup" << std::endl;
        newSettings.close();
        return true;
    }

    return false;
}


// MARK: TEST COMMAND
TestCommand::TestCommand() : Command {"test", "Testing command"}
{

}

TestCommand::~TestCommand(){

}

bool TestCommand::execute() const {
    std::cout << "in test command\n";
    return true;    
}

// MARK: LAS::FrameworkSetup Namespace
namespace LAS::FrameworkSetup{
    std::string createLogFile(const std::string& parentDir){
        std::chrono::zoned_time now { std::chrono::current_zone(), std::chrono::system_clock::now() };

        using namespace TextManipulations::Logging;   // for PrintTime()
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
                std::string directory {"/mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/LAS-Test/"};
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

        if(!LAS::ensureDirectory(filePaths.logDir) || !LAS::ensureDirectory(filePaths.moduleDir)){
            return false;
        }

        return true;
    }

}