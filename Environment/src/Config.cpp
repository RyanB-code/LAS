#include "Config.h"

std::string LAS::Environment::getVersion(){
    std::ostringstream os {};

    os << LAS_ENV_VERSION_MAJOR << "." << LAS_ENV_VERSION_MINOR << "." << LAS_ENV_VERSION_PATCH;
    
    return os.str();
}
std::string LAS::Environment::getInformation(){
    std::ostringstream os;
    
    os << "Life Application Suite\n";
    os << "Developed by Bradley Ryan (C) 2024\n\n";

    os << "Environment version " << getVersion() << "\n";

    return os.str();
}

uint8_t LAS::Environment::getVersionMajor(){
    return LAS_ENV_VERSION_MAJOR;
}
uint8_t LAS::Environment::getVersionMinor(){
    return LAS_ENV_VERSION_MINOR;
}
uint8_t LAS::Environment::getVersionPatch(){
    return LAS_ENV_VERSION_PATCH;
}