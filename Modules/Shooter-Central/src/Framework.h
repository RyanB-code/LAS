#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Drill.h"

#include <string>
#include <stdint.h>
#include <unordered_map>

namespace ShooterCentral{
    class Framework final {
    public:
        Framework();
        ~Framework();

    private:
        std::unordered_map<std::string, Gun>    guns;
        std::vector<Drill>                      drills;
        std::vector<std::pair<uint64_t, Ammo>>  ammoStockpile;
    };
}
