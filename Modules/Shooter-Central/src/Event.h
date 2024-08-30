#pragma once

#include "Gun.h"
#include "Ammo.h"

#include <stdint.h>
#include <string>

namespace ShooterCentral{

    enum class EventType{
        MATCH,
        SELF_TRAINING,
        WORK_RANGE_DAY,
        NOT_APPLICABLE,
    };


    // MARK: MATCH INFO
    class Event final {
    public:
        Event();
        ~Event();

    private:
        std::string name;
        
    };
}