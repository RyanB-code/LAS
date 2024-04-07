#pragma once
#include "Logger.h"
#include "Settings.h"

#include <memory>
#include <string>

class Module{
public:
    Module(std::string setTitle, std::string setShortTitle, LoggerPtr setLogger, Settings setSettings);
    ~Module();

    std::string     getTitle()          const;
    std::string     getShortTitle()     const;
    bool            getShown()          const;
    const Settings& getSettings()       const;

    void            setShown(bool setShown);

private:
    std::string title{};
    std::string shortTitle{};

    bool shown{false};

    LoggerPtr logger{};
    Settings settings{};
    
};

using ModulePtr = std::shared_ptr<Module>;