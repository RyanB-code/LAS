#include "Config.h"

std::string LAS::Environment::getVersion(){
    std::ostringstream os {};

    os << LAS_VERSION_MAJOR << "." << LAS_VERSION_MINOR << "." << LAS_VERSION_PATCH;
    
    return os.str();
}
std::string LAS::Environment::getInformation(){
    std::ostringstream os;
    
    os << "Life Application Suite\n";
    os << "Developed by Bradley Ryan (C) 2024\n";
    os << "Version "    << getVersion()     << "\n";

    return os.str();
}

uint8_t LAS::Environment::getVersionMajor(){
    return LAS_VERSION_MAJOR;
}
uint8_t LAS::Environment::getVersionMinor(){
    return LAS_VERSION_MINOR;
}
uint8_t LAS::Environment::getVersionPatch(){
    return LAS_VERSION_PATCH;
}