#include <LAS/SDK.h>

#include "Config.h"

std::string LAS::SDK::getVersion(){
    std::ostringstream os {};

    os << LAS_SDK_VERSION_MAJOR << "." << LAS_SDK_VERSION_MINOR << "." << LAS_SDK_VERSION_PATCH;
    
    return os.str();
}
std::string LAS::SDK::getInformation(){
    std::ostringstream os;
    
    os << "Life Application Suite SDK\n";
    os << "Developed by Bradley Ryan (C) 2024\n";
    os << "Version "    << getVersion()     << "\n";

    return os.str();
}

uint8_t LAS::SDK::getVersionMajor(){
    return LAS_SDK_VERSION_MAJOR;
}
uint8_t LAS::SDK::getVersionMinor(){
    return LAS_SDK_VERSION_MINOR;
}
uint8_t LAS::SDK::getVersionPatch(){
    return LAS_SDK_VERSION_PATCH;
}