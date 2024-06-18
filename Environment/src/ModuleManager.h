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


using StringVector = std::vector<std::string>;

class ModuleManager final {
public:
    ModuleManager(LoggerPtr setLogger);
    ~ModuleManager();

    bool addModule      (ModulePtr module);
    bool removeModule   (std::string title);
    bool containsModule (std::string title)   const;


    ModulePtr           getModule(std::string title)        const;
    const StringVector  getModuleNames()                    const;

    WindowList          getAllWindows()                     const;


    [[nodiscard]]
    StringVector loadModules(std::string directory, ImGuiContext& context);

private:
    std::unordered_map<std::string, ModulePtr> modules{};
    LoggerPtr logger;
};

using ModuleManagerPtr = std::shared_ptr<ModuleManager>;

namespace LAS::Modules{
    ModulePtr bindFiletoModule(std::string path, LoggerPtr logger, ImGuiContext& context);
}