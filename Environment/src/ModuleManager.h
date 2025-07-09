#pragma once

#include "Module.h"

#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>
#include <LAS/SDK.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>
#include <queue>

#include <iostream>

namespace LAS{

    class ModuleManager final {
    public:
        explicit ModuleManager();
        ~ModuleManager();

        bool addModule      (const ModulePtr& module);
        bool removeModule   (std::string title);
        bool containsModule (std::string title) const;

        ModulePtr   getModule(std::string title) const;
        ModulePtr   getModule(std::unordered_map<std::string, ModulePtr>::const_iterator itr) const;

        std::string         getModuleLoadDirectory  () const;
        std::string         getModuleFilesDirectory () const;
        bool                setModuleLoadDirectory  (const std::string& directory);
        bool                setModuleFilesDirectory (const std::string& directory);

        void loadAllModules  (ImGuiContext& context, StringVector& modulesNotLoaded, std::string loadDirectory="", std::string filesDirectory=""); // Clears the StringVector first
        bool loadModule      (std::string moduleFilesDirectory, ImGuiContext& context, const std::string& fileName);

        void clearModules   ();

        std::unordered_map<std::string, ModulePtr>::const_iterator cbegin() const;
        std::unordered_map<std::string, ModulePtr>::const_iterator cend() const;
    private:
        std::unordered_map<std::string, ModulePtr> modules{};

        std::string moduleLoadDirectory;
        std::string moduleFilesDirectory;

        static constexpr std::string moduleNameSuffix    {".lasm"};    // Every module must end with this to attempt to be added
    };

    using ModuleManagerPtr  = std::shared_ptr<ModuleManager>;
    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path, const ImGuiContext& context);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
        int         compareVersions         (const Version& base, const Version& compare);      // 0 = same major and minor, 1 = compare minor is older, 2 = compare minor is newer,  3 = fatal mismatch
        
        int         verifyModuleInformation (const ModulePtr& module);                          
        // 0 = success, 1 = bad ptr, 2 = title empty, 3 = group empty, 
        // 4 = bad group name, 5 = version warning, module minor is older than SDK, 6 =  5 = version warning, module minor is newer than SDK, 7 = fatal version mismatch               

    }
}
