#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "LoggingInternal.h"

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
                DisplayManagerPtr   setDisplayManager   =nullptr
            );
    ~Framework  ();

    bool setup();
    void run();

private:
    LoggerPtr           logger;
    ModuleManagerPtr    moduleManager;
    DisplayManagerPtr   displayManager;

    FilePaths           filePaths;

    std::unordered_map<std::string, CommandPtr> commands;
    std::queue<std::string>                     commandQueue;  

    void setupCommands();                                           // This is where to instantiate commands
    bool addCommand(std::unique_ptr<Command> command);              // Adds to commands unoredered_map
    bool handleCommandQueue();

    bool            loadModules        (const std::string& modulesDirectory);
    StringVector    loadModuleCommands (const std::string& moduleName);

    bool readSetupFile      (const std::string& path);
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