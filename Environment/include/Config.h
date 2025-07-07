#pragma once

#define LAS_ENV_VERSION_MAJOR 0
#define LAS_ENV_VERSION_MINOR 6
#define LAS_ENV_VERSION_PATCH 0

#include <string>
#include <stdint.h>
#include <sstream>


namespace LAS::Environment{
    std::string getVersion      ();
    std::string getInformation  ();

    uint8_t getVersionMajor     ();
    uint8_t getVersionMinor     ();
    uint8_t getVersionPatch     ();
}
