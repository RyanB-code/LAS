#pragma once

#include <string>
#include <stdint.h>

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
        Gun();
        ~Gun();
    private:
        std::string name;
        uint8_t     totalRounds;
        WeaponType  weaponType;

        // UNFINISHED
    };
}