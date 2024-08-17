#pragma once

#include <string>
#include <vector>
#include <imgui/imgui.h>

#include "LAS/Logging.h"
#include "LAS/Commands.h"
#include "LAS/Window.h"

namespace LAS::Information{
    struct ModuleInfo {
        std::string             title               {};
        std::string             commandGroupName    {};
        Windowing::WindowPtr    window              {};
        std::vector<CommandPtr> commands            {};
    };
    struct EnvironmentInfo {
        std::string     directory   {};
        std::string     rcFilePath  {};
        
        ImGuiContext&       context;
        Logging::LoggerPtr  logger;
    };
}