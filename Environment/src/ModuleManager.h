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

        std::string         getModuleDirectory() const;
        bool                setModuleDirectory(const std::string& directory);

        [[nodiscard]]
        StringVector    loadModules (const std::string& moduleFilesDirectory, ImGuiContext& context, std::string loadDirectory="");
        bool            loadModule  (std::string moduleFilesDirectory, ImGuiContext& context, const std::string& fileName);

    private:
        std::unordered_map<std::string, ModulePtr> modules{};
        const LoggerPtr& logger;

        std::string moduleDirectory;
    };

    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path, const LoggerPtr& logger, const ImGuiContext& context);
        bool        verifyModuleInformation (const EnvironmentInfo& envInfo, const ModulePtr& module);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
    }
}