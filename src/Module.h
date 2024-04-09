#pragma once
#include "Logger.h"
#include "Settings.h"

#include <memory>
#include <string>

typedef bool(*loadFunction)(const Settings&, ModuleInfo&);      // Function pointer for LASM_load()
typedef void(*voidNoParams)();                                  // Function pointer LASM_run() and LASM_cleanup()

class Module{
public:
    Module(LoggerPtr setLogger, loadFunction setLoad, voidNoParams setRun, voidNoParams setCleanup);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    bool            getShown()          const;
    const Settings& getSettings()       const;

    void            setShown(bool setShown);

    bool load(const Settings& settings);
    void run();
    void cleanup();

private:
    std::string title{};
    std::string shortTitle{};

    bool shown{false};

    LoggerPtr logger{};
    Settings settings{};
    
    loadFunction loadPtr{};
    voidNoParams runPtr{};
    voidNoParams cleanupPtr{};
    
};

using ModulePtr = std::shared_ptr<Module>;