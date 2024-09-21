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

        bool        addToRoundCount(TrackedAmmoPtr ammo);

    private:
        std::string name;
        uint64_t    totalRounds;
        WeaponType  weaponType;

        std::unordered_map<std::string, TrackedAmmoPtr> ammoTracker;
    };

    using GunPtr = std::shared_ptr<Gun>;

    class GunTracker{
    public:
        GunTracker(LAS::Logging::LoggerPtr setLogger);
        ~GunTracker();

        bool addGun     (GunPtr gun);
        bool removeGun  (const std::string& key);

        GunPtr getGun   (const std::string& key) const;

    private:
        LAS::Logging::LoggerPtr logger;

        std::map<std::string, GunPtr> guns;
    };

    namespace GunHelper{
        bool    writeGun    (std::string directory, const Gun& gun);
        Gun     readGun     (const std::string& path);

        std::string weaponTypeToStr (const WeaponType& type);
        WeaponType  strToWeaponType (const std::string& string);
    }
}