#pragma once
#include "Logging.h"
#include "ModuleSettings.h"

#include <memory>
#include <string>

typedef bool(*loadFunction)(const ModuleSettings&, ModuleInfo&);      // Function pointer for LASM_load()
typedef void(*voidNoParams)();                                  // Function pointer LASM_run() and LASM_cleanup()

class Module{
public:
    Module(LoggerPtr setLogger, loadFunction setLoad, voidNoParams setRun, voidNoParams setCleanup);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    bool            getShown()          const;
    const ModuleSettings& getSettings()       const;

    void            setShown(bool setShown);

    bool load(const ModuleSettings& settings);
    void run();
    void cleanup();

private:
    std::string title{};
    std::string shortTitle{};

    bool shown{false};

    LoggerPtr logger{};
    ModuleSettings settings{};
    
    loadFunction loadPtr{};
    voidNoParams runPtr{};
    voidNoParams cleanupPtr{};
    
};

using ModulePtr = std::shared_ptr<Module>;