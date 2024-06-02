#pragma once

#include "ModuleManager.h"
#include "Logging.h"
#include "Display.h"
#include "Commands.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>
#include <chrono>

class Framework final{
public:
    Framework(  LoggerPtr           setLogger=nullptr,
                ModuleManagerPtr    setModuleManager=nullptr,
                DisplayManagerPtr   setDisplayManager=nullptr
            );
    ~Framework  ();

    bool setup();
    void run();

private:
    LoggerPtr logger;
    ModuleManagerPtr moduleManager;
    DisplayManagerPtr displayManager;

    std::unordered_map<std::string, CommandPtr> commands;
    std::queue<std::string> commandQueue;  

    void setupCommands();
    bool addCommand(std::unique_ptr<Command> command);
    bool readSetupFile(std::string path);
    bool handleCommandQueue();

    bool loadModules(std::string modulesDirectory);

};

// For testing command functionality
class TestCommand : public Command{
public: 
    TestCommand();
    ~TestCommand();

    bool execute() const override;
};

namespace LAS {
    bool ensureDirectory    (std::string path);
    bool ensureFile         (std::string path);

    std::string createLogFile   (std::string parentDir);
    std::string getParentDir();
}
