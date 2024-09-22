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
        Gun(std::string setName, WeaponType setWeaponType, std::string setCartridge);
        ~Gun();

        std::string getName         () const;
        uint64_t    getRoundCount   () const;
        WeaponType  getWeaponType   () const;
        std::string getCartridge    () const;

        bool    addNewAmmoTypeToRoundCount  (TrackedAmmoPtr ammo);
        bool    addToRoundCount             (const std::string& ammoName, uint64_t amount);

        void    getAllAmmoUsed (std::vector<TrackedAmmo>& ammoUsed) const; // Clears vector before adding elements

    private:
        std::string name;
        uint64_t    totalRounds;
        WeaponType  weaponType;
        std::string cartridge;

        std::unordered_map<std::string, TrackedAmmoPtr> ammoTracker;
    };

    using GunPtr = std::shared_ptr<Gun>;

    class GunTracker{
    public:
        GunTracker(LAS::Logging::LoggerPtr setLogger);
        ~GunTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        uint64_t getGunTotal () const;

        bool addGun     (GunPtr gun);
        bool removeGun  (const std::string& key);

        GunPtr getGun   (const std::string& key) const;

        bool writeAllGuns   () const;
        bool readGuns       ();

    private:
        LAS::Logging::LoggerPtr logger;

        std::map<std::string, GunPtr> guns;

        std::string saveDirectory;
    };

    using GunTrackerPtr = std::shared_ptr<GunTracker>;

    namespace GunHelper{
        bool    writeGun    (std::string directory, const Gun& gun);
        Gun     readGun     (const std::string& path);

        std::string weaponTypeToStr (const WeaponType& type);
        WeaponType  strToWeaponType (const std::string& string);
    }
}