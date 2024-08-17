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
    os << "Built on "   << __TIMESTAMP__    << "\n"; 

    return os.str();
}