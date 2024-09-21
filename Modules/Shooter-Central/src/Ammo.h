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
    };

    using AmmoTypePtr   = std::shared_ptr<AmmoType>;

    struct TrackedAmmo {
        AmmoType    ammoType;
        uint64_t    amount;
    };

    using TrackedAmmoPtr = std::shared_ptr<TrackedAmmo>;

    class AmmoTracker{
    public:
        AmmoTracker(LAS::Logging::LoggerPtr setLogger);
        ~AmmoTracker();

        bool    addAmmoToStockpile      (uint64_t amount,   const std::string& key);
        bool    addNewAmmoToStockpile   (TrackedAmmoPtr ammo);
        bool    removeAmmoFromStockPile (uint64_t amount,   const std::string& key);

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
        std::map<std::string, TrackedAmmoPtr>   ammoStockpile;
        std::map<std::string, std::string>      cartridges;
        std::string saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string cartridgesFile {"Cartridges.json"};
    };

    using AmmoTrackerPtr = std::shared_ptr<AmmoTracker>;

    namespace AmmoHelper{
        bool        writeAmmo   (std::string directory, const TrackedAmmo& ammo);
        TrackedAmmo readAmmo    (const std::string& path);

        bool    writeAllCartridges      (std::string path, const std::vector<std::string>& cartridges);
        bool    readCartridges          (std::string path, StringVector& cartridgeNames);               // Clears vector before adding elements
    }

}