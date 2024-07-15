#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "Module.h"
#include "LoggingInternal.h"

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

    class Set : public Command {
    public:
        Set(    const std::weak_ptr<DisplayManager>,
                const std::weak_ptr<ModuleManager>,
                const std::weak_ptr<Logger>  
        );
        ~Set();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        std::weak_ptr<DisplayManager> displayManager;
        std::weak_ptr<ModuleManager>  moduleManager;
        std::weak_ptr<Logger>         logger;
    };
}
