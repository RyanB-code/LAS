#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "Module.h"
#include "LoggingInternal.h"
#include "Shell.h"

#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/Helper.h>

// For testing command functionality
namespace LAS::Commands{

    class TestCommand : public Command{
    public: 
        TestCommand();
        ~TestCommand();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    };

    class Manual : public Command {
    public:
        Manual(std::weak_ptr<Shell>);
        ~Manual();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<Shell> shell;
    };

    class Set : public Command {
    public:
        Set(    std::weak_ptr<DisplayManager>,
                std::weak_ptr<ModuleManager>,
                std::weak_ptr<Logger>  
        );
        ~Set();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<DisplayManager> displayManager;
        const std::weak_ptr<ModuleManager>  moduleManager;
        const std::weak_ptr<Logger>         logger;
    };
}
