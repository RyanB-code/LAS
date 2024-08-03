#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "LoggingInternal.h"
#include "Shell.h"
#include "CommandsInternal.h"


#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/Helper.h>

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
        std::string moduleDir;

        std::string rcPath;
        std::string commandHistoryPath;
    };
    
    namespace FrameworkSetup{
        std::string     createLogFile       (const std::string& parentDir);
        std::string     getRCPath           ();
        std::string     getExeParentDir     ();
        bool            setupFilesystem     (FilePaths& filePaths);
    }

    class Framework final{
    public:
        Framework(  LoggerPtr           setLogger           =nullptr,
                    ModuleManagerPtr    setModuleManager    =nullptr,
                    DisplayManagerPtr   setDisplayManager   =nullptr,
                    ShellPtr            setLasShell         =nullptr
                );
        ~Framework  ();

        bool setup  ();
        void run    ();


    private:
        LoggerPtr           logger;
        ModuleManagerPtr    moduleManager;
        DisplayManagerPtr   displayManager;
        ShellPtr            shell;

        static constexpr std::string commandGroupName {"las"};

        bool                setupComplete               { false };
        uint16_t            cacheLastNumberOfCommands   { 50 };     // How many previous commands will be added to command history upon startup

        bool setupShell             (const std::string& rcPath, const std::string& commandHistoryPath);
        bool setupLogger            (const std::string& logDir);
        bool setupModuleManager     (const std::string& moduleDir);
        bool setupDisplay           ();
        bool setupInternalWindows   ();
        void setupCommands          ();                            // This is where to instantiate commands



        bool    loadModules             (const std::string& modulesDirectory);
        void    loadModuleWindows       ();
        void    loadAllModuleCommands   ();                                 
        bool    loadModuleCommands      (const std::string& moduleName, StringVector& commandsNotLoaded);

    };
}
