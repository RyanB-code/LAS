#pragma once


#include <LAS/Logging.h>
#include <LAS/Window.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module
#include <memory>
#include <string>



namespace LAS::Modules{
    typedef bool(*loadFunction)(EnvironmentInfo, ModuleInfo&);     // Function pointer for LASM_load()
    typedef void(*voidNoParams)();                              // Function pointer LASM_cleanup()
};

class Module{
public:
    Module( LoggerPtr                    setLogger,
            LAS::Modules::loadFunction   setLoad, 
            LAS::Modules::voidNoParams   setCleanup);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    
    const ModuleInfo&   getInfo()       const;
    std::vector<CommandPtr>&   getCommands();

    bool    load(const EnvironmentInfo& whatToPass);
    void    cleanup();

private:
    ModuleInfo moduleInfo {};
    LoggerPtr logger;

    
    LAS::Modules::loadFunction loadPtr       {};
    LAS::Modules::voidNoParams cleanupPtr    {};
};

using ModulePtr = std::shared_ptr<Module>;