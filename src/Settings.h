#pragma once

#include <string>
#include <cstdint>

struct Settings{
    uint8_t         fontSize        {};
    int8_t          hourOffset      {};
    std::string     directory       {};
};

struct ModuleInfo{
    std::string title       {};
    std::string shortTitle  {};
};