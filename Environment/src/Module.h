#pragma once

#include <LAS/Logging.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>

#include <imgui/imgui_internal.h> 

#include <memory>
#include <string>

using namespace LAS::Information;
using namespace LAS::Logging;

namespace LAS{

    class Module;
    using ModulePtr = std::shared_ptr<Module>;

    namespace ModuleFunctions{
        typedef bool(*LoadModuleInfo)       (ModuleInfo&);      
        typedef bool(*LoadEnvironmentInfo)  (const EnvironmentInfo&); 
        typedef void(*VoidNoParams)         (); 
    };

    class Module{
    public:
        explicit Module(    LAS::ModuleFunctions::LoadModuleInfo            setLoadModuleInfo,
                            LAS::ModuleFunctions::LoadEnvironmentInfo       setLoadEnvironmentInfo,
                            LAS::ModuleFunctions::VoidNoParams              setCleanup
                        );
        ~Module();

        bool    setDirectory    (std::string directory);
        bool    setRCFilePath   (const std::string& path);

        const ModuleInfo& getModuleInfo() const;

        std::string                 getDirectory()      const;
        std::string                 getRCFilePath()     const;

        std::vector<CommandPtr>&    getCommands();

        bool    loadModuleInfo();
        bool    loadEnvInfo(const EnvironmentInfo& envInfo);
        void    cleanup();
        
    private:
        ModuleInfo moduleInfo {};

        std::string directory;
        std::string rcFilePath;

        LAS::ModuleFunctions::LoadModuleInfo        loadModuleInfoPtr   {};
        LAS::ModuleFunctions::LoadEnvironmentInfo   loadEnvInfoPtr      {};
        LAS::ModuleFunctions::VoidNoParams          cleanupPtr          {};
    };
}
