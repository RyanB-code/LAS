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

    /*
        Order of Operations:
        1. Initialize application pieces to their default state
            (Module Manager, Logger, DisplayManager)
        2. Read .las-rc file
            - Parse commands
        3. Handle the commands
        4. 
    
    */

    // Ensure filesystem is in place
    std::string parentDir {LAS::getParentDir()};
    if(!LAS::ensureDirectory(parentDir + "logs/") || !LAS::ensureDirectory(parentDir + "modules/")){
        std::cerr << "Could not create filesystem\n";
        return false;
    }

    // Default Setup
    if(!logger){
        LogSettingsPtr logSettings {new LogSettings{}};                     // Uses default settings values
        logger = std::make_shared<Logger>(Logger{logSettings});             // Sets local logger member variable
        
        LogToFile logToFile{};

    }
    if(!moduleManager){

    }
    if(!displayManager){

    }

    setupCommands();
    if(!readSetupFile(settingsPath)){
        std::cerr << "Error reading settings file\n";
    }
    handleCommandQueue();
    
    return false;
}
void Framework::run(){

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