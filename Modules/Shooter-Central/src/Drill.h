#pragma once

#include "Gun.h"

#include <string>
#include <stdint.h>

namespace ShooterCentral{
    struct TargetConfiguration{
        uint8_t         targetsNeeded;
        std::string     targetSetupDescription;
    };
    struct LoadConfiguration{
        WeaponType      weaponType;
        uint8_t         roundsNeeded;
        uint8_t         magazinesNeeded;
        std::string     loadDescription;
    };
    struct CourseOfFire{
        LoadConfiguration   loadConfiguration;
        TargetConfiguration targetConfiguration;
        std::string         courseOfFireDescription;
    };

    struct Drill{
        std::string         name;
        CourseOfFire        courseOfFire;
        bool                canBeDryFire;
        bool                canBeLiveFire;
    };
}