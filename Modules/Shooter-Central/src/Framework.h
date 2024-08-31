#pragma once

#include "Ammo.h"
#include "Gun.h"
#include "Drill.h"

#include <string>
#include <stdint.h>
#include <unordered_map> 
#include <vector>

namespace ShooterCentral{
    class Framework final {
    public:
        Framework();
        ~Framework();

        bool addGun     (GunPtr gun);
        bool removeGun  (const std::string& key);

        void addDrill   (Drill drill);
         
        bool addAmmoToStockpile         (uint64_t amount,   AmmoPtr ammo);
        bool removeAmmoFromStockPile    (uint64_t amount,   const std::string& key);

    private:
        std::unordered_map<std::string, Gun>                            guns;
        std::unordered_map<std::string, std::pair<uint64_t, AmmoPtr>>   ammoStockpile;

        std::vector<Drill> drills;
    };
}
