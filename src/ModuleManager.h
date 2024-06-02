#pragma once

#include "Logging.h"
#include "Module.h"
#include "LAS.h"

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>


using StringVector = std::vector<std::string>;

class ModuleManager final {
public:
    ModuleManager(const Logger& setLogger);
    ~ModuleManager();

    bool addModule(ModulePtr module);
    bool removeModule(std::string title);

    ModulePtr           getModule(std::string title)        const;
    bool                containsModule(std::string title)   const;
    const StringVector  getModuleNames()                    const;

    [[nodiscard]]
    std::pair<int, StringVector> loadModules(std::string directory, ImGuiContext& context);

private:

    std::unordered_map<std::string, ModulePtr> modules{};

    const Logger&       logger;
};

using ModuleManagerPtr = std::shared_ptr<ModuleManager>;

namespace LAS::Modules{
    ModulePtr bindFiletoModule(std::string path, const Logger& logger, ImGuiContext& context);
}