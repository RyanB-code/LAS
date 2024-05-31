#pragma once

#include "Logging.h"
#include "Module.h"
#include "TextManipulations.h"
#include "ModuleSettings.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <dlfcn.h>

using StringVector = std::vector<std::string>;

class ModuleManager final {
public:
    ModuleManager(LoggerPtr setLogger, ModuleSettingsPtr setSettings);
    ~ModuleManager();

    bool addModule(ModulePtr module);
    bool removeModule(std::string title);

    ModulePtr   getModule(std::string title) const;
    bool        containsModule(std::string title) const;
    const StringVector getModuleNames() const;

    [[nodiscard]]
    std::pair<int, StringVector> loadModules(std::string directory);

private:

    std::unordered_map<std::string, ModulePtr> modules{};

    LoggerPtr logger;
    ModuleSettingsPtr settings;
};

using ModuleManagerPtr = std::shared_ptr<ModuleManager>;

namespace LASCore{
    ModulePtr bindFiletoModule(std::string path, LoggerPtr logger);
}