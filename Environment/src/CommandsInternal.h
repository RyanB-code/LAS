#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "Module.h"
#include "LoggingInternal.h"
#include "Shell.h"
#include "Config.h"

#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/HelperFunctions.h>
#include <LAS/SDK.h>


using namespace LAS::Logging;

// For testing command functionality
namespace LAS::Commands{

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
                std::weak_ptr<Shell>  
        );
        ~Set();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<DisplayManager> displayManager;
        const std::weak_ptr<ModuleManager>  moduleManager;
        const std::weak_ptr<Shell>          shell;
    };

    class Print : public Command {
    public:
        Print ( std::weak_ptr<DisplayManager>,
                std::weak_ptr<ModuleManager>
            );
        ~Print();
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<DisplayManager> displayManager;
        const std::weak_ptr<ModuleManager>  moduleManager;
    };

    class Echo : public Command {
    public:
        Echo ();
        ~Echo();
    
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    };

    class ModuleControl : public Command{
    public:
        ModuleControl   (   std::weak_ptr<DisplayManager>,
                            std::weak_ptr<ModuleManager>,
                            std::weak_ptr<Shell>
                        );
        ~ModuleControl();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<DisplayManager> displayManager;
        const std::weak_ptr<ModuleManager>  moduleManager;
        const std::weak_ptr<Shell>          shell;

    };

    class Information : public Command{
    public:
        Information     ();
        ~Information    ();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    };
    
    class Reload : public Command{
    public:
        Reload(std::weak_ptr<Shell>);
        ~Reload();
        
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<Shell> shell;
    };
    class DisplayControl : public Command{
    public:
        DisplayControl( std::weak_ptr<DisplayManager> );
        ~DisplayControl();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        const std::weak_ptr<DisplayManager> displayManager;
    };  
}
