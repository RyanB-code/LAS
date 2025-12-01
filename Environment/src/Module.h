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
        typedef bool(*InitModule)           (const EnvironmentInfo&); 
        typedef void(*VoidNoParams)         (); 
    }

    struct ModuleUpdate {
        std::string moduleTag;
        std::function<void()>  updateFunction;
    };

    class Module{
    public:
        explicit Module(    LAS::ModuleFunctions::LoadModuleInfo   setLoadModuleInfo,
                            LAS::ModuleFunctions::InitModule       setModuleInit,
                            LAS::ModuleFunctions::VoidNoParams     setCleanup
                        );
        ~Module();

        bool setDirectory    (std::string directory);
        bool setRCFilePath   (const std::string& path);

        const ModuleInfo&   getModuleInfo()     const;
        ModuleUpdate        getModuleUpdate()   const;
        std::string         getDirectory()      const;
        std::string         getRCFilePath()     const;

        std::vector<std::shared_ptr<Command>>&    getCommands();

        bool    loadInfo();
        bool    init(const EnvironmentInfo& envInfo);
        void    cleanup();
        
    private:
        ModuleInfo moduleInfo {};

        std::string directory;
        std::string rcFilePath;

        LAS::ModuleFunctions::LoadModuleInfo    loadModuleInfoPtr   { };
        LAS::ModuleFunctions::InitModule        initModulePtr       { };
        LAS::ModuleFunctions::VoidNoParams      cleanupPtr          { };
    };
}
