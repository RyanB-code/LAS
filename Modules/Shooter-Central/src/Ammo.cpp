#include "Ammo.h"

using namespace ShooterCentral;

// MARK: CARTRIDGE TYPE
CartridgeType::CartridgeType(std::string setName) : name {setName}
{

}
CartridgeType::~CartridgeType(){

}
std::string CartridgeType::getName() const {
    return name;
}

Cartridge_9MM::Cartridge_9MM() : CartridgeType("9mm"){

}
Cartridge_9MM::~Cartridge_9MM(){

}
Cartridge_556::Cartridge_556() : CartridgeType("5.56x45mm"){

}
Cartridge_556::~Cartridge_556(){

}
Cartridge_300WM::Cartridge_300WM() : CartridgeType("300 Win Mag"){

}
Cartridge_300WM::~Cartridge_300WM(){

}

