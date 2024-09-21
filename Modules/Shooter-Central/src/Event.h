#pragma once

#include "Gun.h"
#include "Ammo.h"

#include <stdint.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace ShooterCentral{

    enum class EventType{
        MATCH,
        SELF_TRAINING,
        WORK_RANGE_DAY,
        NOT_APPLICABLE,
    };

    struct EventLocation{
        std::string name;
    };

    // MARK: MATCH INFO
    class Event final {
    public:
        Event(std::string setName, EventLocation setLocation, EventType setEventType, std::string setNotes);
        ~Event();

        std::string     getName()           const;
        std::string     getNotes()          const;
        EventType       getEventType()      const;
        EventLocation   getEventLocation()  const;

       // bool            addGun(GunPtr gun, AmmoPtr ammo, uint64_t rounds);

    private:
        std::string                                         name;
        EventLocation                                       location;
        EventType                                           eventType;
       // std::vector<std::tuple<GunPtr, AmmoPtr, uint64_t>>  gunsUsed;
        std::string                                         notes;
        
    };

    using EventPtr = std::shared_ptr<Event>;
}