#pragma once
#include "Logging.h"
#include "Window.h"

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module
#include <memory>
#include <string>

struct ModuleInfo    {
    std::string     title           {};
    std::string     shortTitle      {};
    WindowPtr       window          {};
};
struct GiveToModule     {
    std::string     directory{};
    
    ImGuiContext&   context;
    const Logger&   logger;
};


namespace LAS::Modules{
    typedef bool(*loadFunction)(GiveToModule, ModuleInfo&);     // Function pointer for LASM_load()
    typedef void(*voidNoParams)();                              // Function pointer LASM_cleanup()
};

class Module{
public:
    Module( const Logger&                setLogger,
            LAS::Modules::loadFunction   setLoad, 
            LAS::Modules::voidNoParams   setCleanup);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    
    const ModuleInfo& getInfo()       const;

    bool    load(const GiveToModule& whatToPass);
    void    cleanup();

private:
    ModuleInfo moduleInfo {};
    const Logger& logger;

    
    LAS::Modules::loadFunction loadPtr       {};
    LAS::Modules::voidNoParams cleanupPtr    {};
};

using ModulePtr = std::shared_ptr<Module>;