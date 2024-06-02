#pragma once
#include "Logging.h"
#include "ModuleSettings.h"

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module


#include <memory>
#include <string>

typedef bool(*loadFunction)(const ModuleSettings&, ModuleInfo&, ImGuiContext&);      // Function pointer for LASM_load()
typedef void(*voidNoParams)();                                  // Function pointer LASM_cleanup()

class Module{
public:
    Module(LoggerPtr setLogger, loadFunction setLoad, voidNoParams setRun, voidNoParams setCleanup);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    bool&           show();
    const ModuleSettings& getSettings()       const;

    bool load(const ModuleSettings& settings, ImGuiContext& context);
    void run();
    void cleanup();

private:
    std::string title{};
    std::string shortTitle{};

    bool* shown {};

    LoggerPtr logger{};
    ModuleSettings settings{};
    
    loadFunction loadPtr{};
    voidNoParams runPtr{};
    voidNoParams cleanupPtr{};
    
};

using ModulePtr = std::shared_ptr<Module>;