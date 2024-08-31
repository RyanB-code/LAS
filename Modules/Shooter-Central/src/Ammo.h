#pragma once

#include <stdint.h>
#include <string>
#include <memory>

namespace ShooterCentral{  

    // MARK: AMMO INFO
    class CartridgeType {
    public:
        CartridgeType(std::string setName);
        virtual ~CartridgeType();

        std::string getName() const;

    private:
        std::string name;
    };
    class Cartridge_9MM : public CartridgeType {
    public:
        Cartridge_9MM();
        ~Cartridge_9MM();
    };
    class Cartridge_556 : public CartridgeType {
    public:
        Cartridge_556();
        ~Cartridge_556();
    };
    class Cartridge_300WM : public CartridgeType {
    public:
        Cartridge_300WM();
        ~Cartridge_300WM();
    };

    struct Ammo {
        std::string     name;
        std::string     manufacturer;
        CartridgeType   cartridgeType;     
        uint8_t         grainWeight;
    };

    using AmmoPtr = std::shared_ptr<Ammo>;
}