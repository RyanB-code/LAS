#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "LoggingInternal.h"
#include "Shell.h"

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
        std::string settingsPath;

        std::string parentDir;
        std::string logDir;
        std::string moduleDir;
    };

    namespace FrameworkSetup{
        std::string     createLogFile   (const std::string& parentDir);
        std::string     getSettingsPath ();
        std::string     getExeParentDir ();

        bool            setupFilesystem (FilePaths& filePaths);
    }

    class Framework final{
    public:
        Framework(  LoggerPtr           setLogger           =nullptr,
                    ModuleManagerPtr    setModuleManager    =nullptr,
                    DisplayManagerPtr   setDisplayManager   =nullptr,
                    ShellPtr            setLasShell         =nullptr
                );
        ~Framework  ();

        bool setup();
        void run();


    private:
        LoggerPtr           logger;
        ModuleManagerPtr    moduleManager;
        DisplayManagerPtr   displayManager;
        ShellPtr            lasShell;

        LAS::FilePaths      filePaths;
        bool                setupComplete{false};


        bool setupShell();
        void setupCommands();                                           // This is where to instantiate commands
        bool setupLogger();
        bool setupModuleManager();
        bool setupDisplay();
        bool setupInternalWindows();


        bool            loadModules             (const std::string& modulesDirectory);
        void            loadModuleWindows       ();
        void            loadAllModuleCommands   ();                                 
        StringVector    loadModuleCommands      (const std::string& moduleName);

    };
}

// For testing command functionality
class TestCommand : public Command{
public: 
    TestCommand();
    ~TestCommand();

    std::pair<int, std::ostringstream> execute(const StringVector&) override;
};