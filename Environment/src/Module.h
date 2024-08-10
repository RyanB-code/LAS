#pragma once

#include <LAS/Logging.h>
#include <LAS/Window.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>

#include <imgui/imgui_internal.h> 

#include <memory>
#include <string>

namespace LAS{

    class Module;
    using ModulePtr = std::shared_ptr<Module>;

    namespace Modules{
        typedef bool(*LoadModuleInfo)       (ModuleInfo&);      
        typedef bool(*LoadEnvironmentInfo)  (const EnvironmentInfo&); 
        typedef void(*VoidNoParams)         (); 
    };

    class Module{
    public:
        explicit Module( const LoggerPtr&               setLogger,
                LAS::Modules::LoadModuleInfo            setLoadModuleInfo,
                LAS::Modules::LoadEnvironmentInfo       setLoadEnvironmentInfo,
                LAS::Modules::VoidNoParams              setCleanup);
        ~Module();

        std::string     getTitle()          const;
        std::string     getGroupName()      const;
        
        const ModuleInfo&   getInfo()       const;
        std::vector<CommandPtr>&   getCommands();

        bool    loadModuleInfo();
        bool    loadEnvInfo(const EnvironmentInfo& envInfo);
        void    cleanup();
        

    private:
        ModuleInfo moduleInfo {};
        const LoggerPtr& logger;

        LAS::Modules::LoadModuleInfo        loadModuleInfoPtr   {};
        LAS::Modules::LoadEnvironmentInfo   loadEnvInfoPtr      {};
        LAS::Modules::VoidNoParams          cleanupPtr          {};
    };
}
