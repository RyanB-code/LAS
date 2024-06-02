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

    // If linux, check the home directory
    #ifdef __linux__

        #ifdef DEBUG
            std::string directory {"/mnt/NAS/1-Project-Related/Project-Source-Directories/LAS/LAS-Test/"};
            std::string settingsPath {directory + ".las-rc"};
        #endif
        #ifndef DEBUG
            std::string home {getenv("HOME")};
            std::string settingsPath { home + "/.las-rc"};
        #endif
    #endif

    // If windows, right now say its not supported
     #ifdef _Win32
        std::cerr << "Windows is not currently supported.\nApplication aborted.\n";
        return -1;
    #endif


    // Ensure filesystem is in place
    std::string parentDir   {LAS::getParentDir()};
    std::string logDir      {parentDir + "logs/"};
    std::string modulesDir  {parentDir + "modules/"};
    if(!LAS::ensureDirectory(logDir) || !LAS::ensureDirectory(modulesDir)){
        std::cerr << "Could not create filesystem\n";
        return false;
    }

    // Default Setup of Logger
    if(!logger){
        LogSettingsPtr logSettings {new LogSettings{}};                     // Uses default settings values
        logger = std::make_shared<Logger>(Logger{logSettings});             // Sets local logger member variable
        
        LogToFile logToFile{};
        logToFile.setPath(LAS::createLogFile(logDir));

        // Check to ensure log file is good
        if(logToFile.getPath().empty()){
            std::cerr << "Could not create instance log file\n";
            return false;
        }

        // If all is good, add LogToFile to logger
        logger->addOutput(std::make_shared<LogToFile>(logToFile));
    }
    // After logging is setup, use logger for messages

    if(!moduleManager){
        ModuleSettingsPtr moduleSettings { new ModuleSettings { modulesDir }};
        moduleManager = std::make_shared<ModuleManager>(  ModuleManager{logger, moduleSettings});
    }
    if(!displayManager){
        displayManager = std::make_shared<DisplayManager>(*logger.get(), moduleManager);
        if(!displayManager->init()){
            logger->log("Error setting up necessary display libraries", Tags{"Display Manager"});
            return false;
        }
    }

    setupCommands();
    if(!readSetupFile(settingsPath)){
        logger->log("Error reading settings file at [" + settingsPath + "]", Tags{"Filesystem Error"});
        return false;
    }
    handleCommandQueue();

    // After all is setup, load modules
    if(!loadModules(modulesDir)){
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
   std::unique_ptr<TestCommand> testCommand {new TestCommand()};

   addCommand(std::move(testCommand));
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
bool Framework::readSetupFile(std::string path){

     if(std::filesystem::exists(path)){
        std::ifstream settingsFile {path}; 
        std::string line;

        // Read line by line
        while (std::getline(settingsFile, line))
        {
            std::stringstream inputStream{};
            inputStream << line;
            std::string buffer {inputStream.str()};

            // Filter out comments and newlines, add the rest
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
bool Framework::handleCommandQueue(){

    std::cout << "Commands: \n";
    for (; !commandQueue.empty(); commandQueue.pop())
        std::cout << commandQueue.front() << '\n';
    std::cout << "\nEnd commands. End size: " << commandQueue.size() << "\n";

    /* This is for inputting quoted buffer
    while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
            arguments.push_back(buffer);                // Add to argument list
        }
    */
    
    
    return false;
}
bool Framework::loadModules(std::string modulesDirectory) {
    if(!ImGui::GetCurrentContext()){
        logger->log("No ImGuiContext found", Tags{"Module Loader"});
        return false;
    }


    try{
        std::pair<int, StringVector> modulesThatFailedToLoad {moduleManager->loadModules(modulesDirectory, *ImGui::GetCurrentContext())}; 
        std::ostringstream msg;
        msg << "There were [" << modulesThatFailedToLoad.first << "] Modules that could not be loaded: ";
        for(const auto& s : modulesThatFailedToLoad.second){
            msg << "[" << s << "] ";
        }
        logger->log(msg.str(), Tags{"Module Manager"});
        return true;
    }
    catch(std::filesystem::filesystem_error& e){
        logger->log("Could not find Module directory [" + modulesDirectory + "] to load Modules", Tags{"Module Manager", "Filesystem Error"});
        return false;
    }
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

// MARK: LAS Namespace
namespace LAS{
    bool ensureDirectory (std::string path){
        if(std::filesystem::exists(path)){
            return true;
        }
        else{
            return std::filesystem::create_directories(path);
        }
    }
    bool ensureFile     (std::string path){
        if(std::filesystem::exists(path)){
            return true;
        }
        else{
            std::ofstream file(path);
            return std::filesystem::exists(path);
        }
    }
    std::string createLogFile(std::string parentDir){
        std::chrono::zoned_time now { std::chrono::current_zone(), std::chrono::system_clock::now() };

        using namespace TextManipulations::Logging;   // for PrintTime()
        std::string fileName {parentDir + std::format("{:%F} {}", now, printTime(now))};  // Outputs in format HH:MM:SS DD MM YYYY as 19:56::23 28 05 2024

        if(!ensureFile(fileName))
            return "";
        else
            return fileName;
    }
    std::string getParentDir(){
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
}