#pragma once

#include "Ammo.h"

#include <string>
#include <stdint.h>
#include <memory>
#include <unordered_map>

//MARK: GUN
namespace ShooterCentral{

    enum class WeaponType{
        PISTOL,
        RIFLE,
        PRECISION_RIFLE,
        NOT_APPLICABLE
    };  


    class Gun final {
    public:
        Gun(std::string setName, WeaponType setWeaponType);
        ~Gun();

        std::string getName()       const;
        uint64_t    getRoundCount() const;
        WeaponType  getWeaponType() const;

        void        addToRoundCount(uint64_t add, const Ammo& ammoUsed);

    private:
        std::string name;
        uint64_t    totalRounds;
        WeaponType  weaponType;

        std::unordered_map<std::string, std::pair <uint64_t, const Ammo&>> ammoTracker;
    };

        using GunPtr = std::shared_ptr<Gun>;
}