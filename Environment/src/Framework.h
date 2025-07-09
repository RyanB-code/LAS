#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "Loggers.h"
#include "Shell.h"
#include "CommandsInternal.h"


#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/HelperFunctions.h>

#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include <queue>
#include <chrono>
#include <exception>

namespace LAS{

    struct FilePaths{
        std::string parentDir;
        std::string dotLASDir;

        std::string logDir;
        std::string moduleLibDir;
        std::string moduleFilesDir;

        std::string rcPath;
        std::string commandHistoryPath;

        std::string imGuiIniPath;
    };
    
    namespace FrameworkSetup{
        std::string     createLogFile       (const std::string& parentDir);
        std::string     getExeParentDir     ();

        bool            setupFileLogger     (const std::string& logDir);
        bool            setupFilesystem     (FilePaths& filePaths);
    }

    class Framework final{
    public:
        Framework();
        ~Framework();

        bool setup  ();
        void run    ();

    private:
        static constexpr char       COMMAND_GROUP_NAME[4]   {"las"};    // Group for all built in commands
        static constexpr int16_t    NUM_CACHED_COMMANDS     { 50 };     // How many previous commands will be added to command history upon startup
        static constexpr uint8_t    MAX_MODULES             { 32 };

        bool setupComplete { false };

        std::shared_ptr<ModuleManager>      moduleManager;
        std::shared_ptr<DisplayManager>     displayManager;
        std::shared_ptr<Shell>              shell;
        std::shared_ptr<Display::LogWindow> logWindow { }; // Needed to store logs
                                                           
        std::array<std::function<void()>, MAX_MODULES> updateFunctions;
        int nextIndex { 0 };
        bool addUpdateFunction(std::function<void()> func);
        std::array<std::function<void()>, MAX_MODULES>::const_iterator modifiedEnd();


        std::pair<bool, int> setupLoggers();

        bool setupShell             (const std::string& rcPath, const std::string& commandHistoryPath);
        bool setupModuleManager     (const std::string& moduleLoadDir, const std::string& moduleFilesDir);
        bool setupDisplay           (const std::string& iniFilePath);
        bool setupInternalWindows   ();
        void setupCommands          (); // Instantiate commands here                            

        bool loadAllModules         (const std::string& moduleLibDirectory, const std::string& moduleFilesDirectory);
        void loadAllModuleFunctions ();
        void loadAllModuleCommands  ();                                 
        void setupAllModules        ();
                                        
        void readAllModuleRCFiles   ();
        bool readModuleRCFile       (const std::string& name);

        void removeModule           (std::unordered_map<std::string, ModulePtr>::const_iterator itr); // Goes through all managers and deletes information
    };
}
