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
        Gun(std::string setName, WeaponType setWeaponType, std::string setCartridge, uint16_t setID);
        ~Gun();

        std::string getName         () const;
        uint64_t    getRoundCount   () const;
        WeaponType  getWeaponType   () const;
        std::string getCartridge    () const;
        uint16_t    getID           () const;

        bool    addToRoundCount (TrackedAmmoPtr ammo);
        bool    addToRoundCount (const std::string& ammoName, uint64_t amount);

        void    getAllAmmoUsed (std::vector<TrackedAmmo>& ammoUsed) const; // Clears vector before adding elements

    private:
        std::string name;
        WeaponType  weaponType;
        std::string cartridge;
        const uint16_t ID;

        std::unordered_map<std::string, TrackedAmmoPtr> ammoTracker;
    };

    using GunPtr = std::shared_ptr<Gun>;

    class GunFactory final {
    public:
        GunFactory();
        ~GunFactory();

        Gun createGun (const std::string& name, WeaponType weaponType, std::string cartridge) const;
    private:
        static uint16_t gunIDNumber;
    };

    class GunTracker{
    public:
        GunTracker(LAS::Logging::LoggerPtr setLogger);
        ~GunTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        uint16_t    createPistol            (const std::string& name, const std::string& cartridge);    // Returns assigned ID, 0 means it was not added
        uint16_t    createRifle             (const std::string& name, const std::string& cartridge);    // Returns assigned ID, 0 means it was not added
        uint16_t    createPrecisionRifle    (const std::string& name, const std::string& cartridge);    // Returns assigned ID, 0 means it was not added
        bool        removeGun               (const uint16_t& ID);

        uint64_t    getGunTotal ()                              const;
        GunPtr      getGun      (const uint16_t& ID)            const;
        void        getAllGunIDs(std::vector<uint16_t>& list) const;  // Clears vector before adding elements 

        bool writeAllGuns   () const;
        bool readGuns       ();

    private:
        LAS::Logging::LoggerPtr     logger;
        GunFactory                  gunFactory;

        std::map<uint16_t, GunPtr> guns;

        std::string saveDirectory;

        bool addGun(GunPtr gun);
    };

    using GunTrackerPtr = std::shared_ptr<GunTracker>;

    namespace GunHelper{
        bool    writeGun    (std::string directory, const Gun& gun);
        Gun     readGun     (const std::string& path);

        std::string weaponTypeToStr (const WeaponType& type);
        WeaponType  strToWeaponType (const std::string& string);
    }
}