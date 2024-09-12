#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Drill.h"
#include "Event.h"
#include "Window.h"

#include <LAS/Logging.h>

#include <string>
#include <stdint.h>
#include <map> 
#include <vector>

namespace ShooterCentral{
    class Framework final {
    public:
        Framework();
        ~Framework();

        bool addGun         (GunPtr gun);
        bool removeGun      (const std::string& key);

        bool addDrill       (DrillPtr drill);
        bool removeDrill    (const std::string& key);

        bool addEvent       (EventPtr event);
        bool removeEvent    (const std::string& key);
         
        bool addAmmoToStockpile         (uint64_t amount,   AmmoPtr ammo);
        bool removeAmmoFromStockPile    (uint64_t amount,   const std::string& key);

        SCWindowPtr getWindow() const;
        std::string getCommandGroupName() const;

        bool                    setLogger(LAS::Logging::LoggerPtr setLogger);
        LAS::Logging::LoggerPtr getLogger() const;

    private:
        std::map<std::string, EventPtr>                         events;
        std::map<std::string, DrillPtr>                         drills;
        std::map<std::string, GunPtr>                           guns;
        std::map<std::string, std::pair<uint64_t, AmmoPtr>>     ammoStockpile;

        SCWindowPtr window;
        LAS::Logging::LoggerPtr logger;

        static constexpr  std::string commandGroupName { "sc" };
    };
}
