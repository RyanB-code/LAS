#pragma once

#include "Logger.h"
#include "Module.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

using StringVector = std::vector<std::string>;


class ModuleManager final {
public:
    ModuleManager(std::string setModduleDirectory, std::shared_ptr<Logger> setLogger);
    ~ModuleManager();

    bool addModule(ModulePtr module);
    bool removeModule(std::string name);

    ModulePtr   getModule(std::string name);
    bool        findModule(std::string name);

    std::pair<int, StringVector> loadModules();

private:

    std::unordered_map<std::string, ModulePtr> modules{};

    std::string moduleDirectory{};
    std::shared_ptr<Logger> logger;


};