#pragma once

#include <string>
#include <vector>
#include <imgui/imgui.h>

#include "LAS/Logging.h"
#include "LAS/Commands.h"
#include "LAS/Window.h"

struct ModuleInfo    {
    std::string     title               {};
    std::string     commandGroupName    {};
    WindowPtr       window              {};

    std::vector<CommandPtr> commands {};
};
struct EnvironmentInfo     {
    std::string     directory{};
    
    ImGuiContext&   context;
    LoggerPtr       logger;
};