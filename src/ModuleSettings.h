#pragma once

#include <string>
#include <cstdint>
#include <memory>


struct ModuleSettings{
    std::string     directory       {};
};

struct ModuleInfo{
    std::string title       {};
    std::string shortTitle  {};
};

using ModuleSettingsPtr = std::shared_ptr<ModuleSettings>;
