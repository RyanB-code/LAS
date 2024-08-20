#pragma once

#include "Module.h"

#include <LAS/Helper.h>
#include <LAS/Logging.h>
#include <LAS/SDK.h>

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

        static constexpr std::string moduleNameSuffix    {".lasm"};    // Every module must end with this to attempt to be added
    };

    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path, const LoggerPtr& logger, const ImGuiContext& context);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
        int         compareVersions         (const Version& base, const Version& compare);      // 0 = success, 1 = warning mismatch, 2 = fatal mismatch
        int         verifyModuleInformation (const ModulePtr& module);                          // 0 = success, 1 = bad ptr, 2 = title empty, 3 = group empty, 4 = bad group name, 5 = version warning, 6 = fatal version mismatch               

    }
}