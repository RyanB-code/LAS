#pragma once

#include <string>
#include <imgui/imgui.h>
#include <functional>
#include <memory>

#include "LAS/Commands.h"

namespace LAS{

    namespace Information{
        struct Version{
            uint8_t major;
            uint8_t minor;
            uint8_t patch;
        };
        struct ModuleInfo {
            Version     sdkVersion          { };
            Version     moduleVersion       { };

            std::string title               { };
            std::string shortTag            { };

            std::vector<std::shared_ptr<Command>> commands { };
 
            std::function<void()>     updateFunction;
            std::function<void()>     drawFunction;
       };
        struct EnvironmentInfo {
            std::string     directory   {};
            std::string     rcFilePath  {};
        
            ImGuiContext&           context;
        };

        std::string versionToStr (const Version& ver);
    }
}
