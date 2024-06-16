#pragma once


#include <LAS/Logging.h>
#include <LAS/Window.h>
#include <LAS/Commands.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module
#include <memory>
#include <string>

struct ModuleInfo    {
    std::string     title           {};
    std::string     shortTitle      {};
    WindowPtr       window          {};

    std::vector<Command> commands {};
};
struct EnvironmentInfo     {
    std::string     directory{};
    
    ImGuiContext&   context;
    LoggerPtr       logger;
};


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
    std::vector<Command>&   getCommands();

    bool    load(const EnvironmentInfo& whatToPass);
    void    cleanup();

private:
    ModuleInfo moduleInfo {};
    LoggerPtr logger;

    
    LAS::Modules::loadFunction loadPtr       {};
    LAS::Modules::voidNoParams cleanupPtr    {};
};

using ModulePtr = std::shared_ptr<Module>;