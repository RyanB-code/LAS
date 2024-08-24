#pragma once

#include <string>
#include <vector>
#include <imgui/imgui.h>

#include "LAS/Logging.h"
#include "LAS/Commands.h"
#include "LAS/Window.h"

namespace LAS::Information{
    struct Version{
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    };
    struct ModuleInfo {
        Version                 sdkVersion          {};
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

    std::string versionToStr (const Version& ver);
}