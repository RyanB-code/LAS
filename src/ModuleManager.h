#pragma once

#include "Logger.h"
#include "Module.h"
#include "TextManipulations.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include <iostream>

using StringVector = std::vector<std::string>;


class ModuleManager final {
public:
    ModuleManager(std::shared_ptr<Logger> setLogger);
    ~ModuleManager();

    bool addModule(ModulePtr module);
    bool removeModule(std::string title);

    ModulePtr   getModule(std::string title) const;
    bool        containsModule(std::string title) const;

    std::pair<int, StringVector> loadModules(std::string directory);

private:

    std::unordered_map<std::string, ModulePtr> modules{};

    std::shared_ptr<Logger> logger;


};