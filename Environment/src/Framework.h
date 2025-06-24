#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "LoggingInternal.h"
#include "Shell.h"
#include "CommandsInternal.h"


#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/HelperFunctions.h>

#include <iostream>
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
        
        std::pair<bool, int>    setupBasicLogger();     // Return: (bool = function success, int = log output ID)
    }

    class Framework final{
    public:
        Framework(  ModuleManagerPtr    setModuleManager    =nullptr,
                    DisplayManagerPtr   setDisplayManager   =nullptr,
                    ShellPtr            setLasShell         =nullptr
                );
        ~Framework  ();

        bool setup  ();
        void run    ();

    private:
        ModuleManagerPtr    moduleManager;
        DisplayManagerPtr   displayManager;
        ShellPtr            shell;

        static constexpr char       COMMAND_GROUP_NAME[4]  {"las"};     // Group for all built in commands
        static constexpr int16_t    NUM_CACHED_COMMANDS  { 50 };        // How many previous commands will be added to command history upon startup

        bool                setupComplete               { false };

        bool setupShell             (const std::string& rcPath, const std::string& commandHistoryPath);
        bool setupModuleManager     (const std::string& moduleLoadDir, const std::string& moduleFilesDir);
        bool setupDisplay           (const std::string& iniFilePath);
        bool setupInternalWindows   ();
        void setupCommands          (); // Instantiate commands here                            

        bool loadAllModules         (const std::string& moduleLibDirectory, const std::string& moduleFilesDirectory);
        void loadAllModuleWindows   ();
        bool loadModuleWindow       (const std::string& name);
        void loadAllModuleCommands  ();                                 
        bool loadModuleCommands     (const std::string& moduleName, StringVector& commandsNotLoaded);

        void readAllModuleRCFiles   ();
        bool readModuleRCFile       (const std::string& name);
    };
}
