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

struct FilePaths{
    std::string settingsPath;

    std::string parentDir;
    std::string logDir;
    std::string moduleDir;
};

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

    FilePaths           filePaths;
    bool                setupComplete{false};


    bool setupShell();
    void setupCommands();                                           // This is where to instantiate commands
    bool setupLogger();
    bool setupModuleManager();
    bool setupDisplay();
    bool setupInternalWindows();


    bool            loadModules         (const std::string& modulesDirectory);
    void            loadModuleWindows   ();
    void            loadModuleCommands  ();
    StringVector    loadModuleCommands  (const std::string& moduleName);

};

namespace LAS::FrameworkSetup{
    std::string     createLogFile   (const std::string& parentDir);
    std::string     getSettingsPath ();
    std::string     getExeParentDir ();

    bool            setupFilesystem (FilePaths& filePaths);
}

// For testing command functionality
class TestCommand : public Command{
public: 
    TestCommand();
    ~TestCommand();

    bool execute() const override;
};