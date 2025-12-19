#pragma once

#include "Display.h"
#include "ModuleManager.h"
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
        Manual(std::shared_ptr<Shell> shell);
        ~Manual() = default;

        ReturnStatus execute(const StringVector&) override;
    private:
        std::weak_ptr<Shell> weakShell;
    };

    class ModuleControl : public Command{
        public:
            ModuleControl(
                    std::shared_ptr<DisplayManager>, 
                    std::shared_ptr<ModuleManager>, 
                    std::shared_ptr<Shell>
                );
            ~ModuleControl() = default;

            ReturnStatus execute(const StringVector&) override;
        private:
            std::weak_ptr<DisplayManager> weakDM;
            std::weak_ptr<ModuleManager>  weakMM;
            std::weak_ptr<Shell>          weakShell;
    };

    /*
    class Set : public Command {
    public:
        Set(std::shared_ptr<DisplayManager>, std::shared_ptr<ModuleManager>, std::shared_ptr<Shell>);
        ~Set();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        std::shared_ptr<DisplayManager> displayManager;
        std::shared_ptr<ModuleManager>  moduleManager;
        std::shared_ptr<Shell>          shell; 
    };

    class Print : public Command {
    public:
        Print (std::shared_ptr<DisplayManager>, std::shared_ptr<ModuleManager>);
        ~Print();
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        std::shared_ptr<DisplayManager> displayManager;
        std::shared_ptr<ModuleManager>  moduleManager;
    };

    class Echo : public Command {
    public:
        Echo ();
        ~Echo();
    
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    };


    class Information : public Command{
    public:
        Information     ();
        ~Information    ();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    };
    
    class Reload : public Command{
    public:
        Reload(std::shared_ptr<Shell>);
        ~Reload();
        
        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        std::shared_ptr<Shell>          shell;
    };
    class DisplayControl : public Command{
    public:
        DisplayControl(std::shared_ptr<DisplayManager> );
        ~DisplayControl();

        std::pair<int, std::ostringstream> execute(const StringVector&) override;
    private:
        std::shared_ptr<DisplayManager> displayManager;
    };  
    */
}
