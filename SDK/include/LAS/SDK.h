#pragma once

#include <sstream>
#include <stdint.h>

namespace LAS::SDK {
    std::string getVersion      ();
    std::string getInformation  ();

    uint8_t getVersionMajor     ();
    uint8_t getVersionMinor     ();
    uint8_t getVersionPatch     ();
}
