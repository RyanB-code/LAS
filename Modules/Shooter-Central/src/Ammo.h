#pragma once

#include <LAS/json.h>
#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>

#include <stdint.h>
#include <string>
#include <memory>
#include <map>
#include <vector>



namespace ShooterCentral{

    struct AmmoType {
        std::string     name;
        std::string     manufacturer;
        std::string     cartridge;     
        uint8_t         grainWeight;

        bool operator==(const AmmoType& other) const;
    };

    using AmmoTypePtr   = std::shared_ptr<AmmoType>;

    struct TrackedAmmo {
        AmmoType    ammoType;
        uint64_t    amount;
    };

    using TrackedAmmoPtr = std::shared_ptr<TrackedAmmo>;

}

namespace std{

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    template <>
    struct hash<ShooterCentral::AmmoType> {
        size_t operator()(const ShooterCentral::AmmoType& ammo) const{
            std::size_t seed { 0 };

            std::hash_combine(seed, ammo.name);
            std::hash_combine(seed, ammo.manufacturer);
            std::hash_combine(seed, ammo.cartridge);
            std::hash_combine(seed, ammo.grainWeight);

            return seed;
        }
    };
}

namespace ShooterCentral{

    class AmmoTracker{
    public:
        AmmoTracker(LAS::Logging::LoggerPtr setLogger);
        ~AmmoTracker();

        bool    addAmmoToStockpile      (uint64_t amount,   const AmmoType& ammoType);
        bool    removeAmmoFromStockPile (uint64_t amount,   const AmmoType& ammoType);

        void    getAllAmmoNames         (StringVector& names)               const;          // Clears vector before adding elements
        void    getAllAmmo              (std::vector<TrackedAmmo>& list)    const;          // Clears vector before adding elements, only gives you copies of Ammo objects

        void    getAllCartridgeNames    (StringVector& names) const;                                                // Clears vector before adding elements
        void    getAmmoCountByCartridge (std::vector<std::pair<std::string, uint64_t>>& count) const;               // Clears vector before adding elements
        void    getAllAmmoByCartridge   (std::vector<TrackedAmmo>& list, const std::string& cartridgeName)   const; // Clears vector before adding elements, gives copies of ammo objects, adds if cartridge name is equal

        bool    addCartridge            (const std::string& cartridge);

        bool    writeAllAmmo            () const;
        bool    readAllAmmo             ();

        bool    writeAllCartridges      () const;
        bool    readCartridges          ();

        bool        setDirectory        (std::string directory);
        std::string getDirectory        () const;

    private:
        std::unordered_map<AmmoType, TrackedAmmoPtr>      ammoStockpile;
        std::map<std::string, std::string>      cartridges;

        std::string             saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string cartridgesFile {"Cartridges.json"};
    };

    using AmmoTrackerPtr = std::shared_ptr<AmmoTracker>;

    namespace AmmoHelper{
        LAS::json   ammoTypeToJson   (const AmmoType& ammo);
        AmmoType    jsonToAmmoType   (const LAS::json& j);

        bool        writeTrackedAmmo    (std::string directory, const TrackedAmmo& ammo);
        TrackedAmmo readTrackedAmmo     (const std::string& path);

        bool    writeAllCartridges      (std::string path, const std::vector<std::string>& cartridges);
        bool    readCartridges          (std::string path, StringVector& cartridgeNames);               // Clears vector before adding elements
    }

}