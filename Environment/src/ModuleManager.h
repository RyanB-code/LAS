#pragma once

#include "Module.h"

#include <LAS/Helper.h>
#include <LAS/Logging.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>

namespace LAS{
    class ModuleManager;

    using StringVector      = std::vector<std::string>;
    using ModuleManagerPtr  = std::shared_ptr<ModuleManager>;


    class ModuleManager final {
    public:
        explicit ModuleManager(const LoggerPtr& setLogger);
        ~ModuleManager();

        bool addModule      (const ModulePtr& module);
        bool removeModule   (std::string title);
        bool containsModule (std::string title)   const;

        ModulePtr           getModule(std::string title)        const;
        const StringVector  getModuleNames()                    const;

        WindowList          getAllWindows()                     const;

        std::string         getModuleLoadDirectory  () const;
        std::string         getModuleFilesDirectory () const;
        bool                setModuleLoadDirectory  (const std::string& directory);
        bool                setModuleFilesDirectory (const std::string& directory);

        void loadAllModules  (ImGuiContext& context, StringVector& modulesNotLoaded, std::string loadDirectory="", std::string filesDirectory=""); // Clears the StringVector first
        bool loadModule      (std::string moduleFilesDirectory, ImGuiContext& context, const std::string& fileName);

        void clearNonUtilityModules    ();

    private:
        std::unordered_map<std::string, ModulePtr> modules{};
        const LoggerPtr& logger;

        std::string moduleLoadDirectory;
        std::string moduleFilesDirectory;
    };

    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path, const LoggerPtr& logger, const ImGuiContext& context);
        bool        verifyModuleInformation (const EnvironmentInfo& envInfo, const ModulePtr& module);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
    }
}