#pragma once

#include <LAS/Logging.h>
#include <LAS/Window.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>

#include <imgui/imgui_internal.h> 

#include <memory>
#include <string>

using namespace LAS::Information;
using namespace LAS::Logging;
using namespace LAS::Windowing;

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

        bool    setDirectory    (std::string directory);
        bool    setRCFilePath   (const std::string& path);

        const Version&              getSDKVersion()     const;
        const Version&              getModuleVersion()  const;
        std::string                 getTitle()          const;
        std::string                 getGroupName()      const;
        std::string                 getDirectory()      const;
        std::string                 getRCFilePath()     const;
        WindowPtr                   getWindow()         const;

        std::vector<CommandPtr>&    getCommands();

        bool    loadModuleInfo();
        bool    loadEnvInfo(const EnvironmentInfo& envInfo);
        void    cleanup();
        
    private:
        ModuleInfo moduleInfo {};

        std::string directory;
        std::string rcFilePath;

        const LoggerPtr& logger;

        LAS::Modules::LoadModuleInfo        loadModuleInfoPtr   {};
        LAS::Modules::LoadEnvironmentInfo   loadEnvInfoPtr      {};
        LAS::Modules::VoidNoParams          cleanupPtr          {};
    };
}
