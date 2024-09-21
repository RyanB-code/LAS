#include "Event.h"

using namespace ShooterCentral;

Event::Event(std::string setName, EventLocation setLocation, EventType setEventType, std::string setNotes)
    :   name        { setName },
        location    { setLocation },
        eventType   { setEventType },
        notes       { setNotes }
{

}
Event::~Event(){

}
std::string Event::getName() const{
    return name;
}
std::string Event::getNotes() const{
    return notes;
}
EventType Event::getEventType() const{
    return eventType;
}
EventLocation Event::getEventLocation() const{
    return location;
}
/*
bool Event::addGun(GunPtr gun, AmmoPtr ammo, uint64_t rounds){
    if(!gun || !ammo || rounds == 0)
        return false;
    
    // Add to round counter for gun
    if(!gun->addToRoundCount(ammo, rounds))
        return false;
    
    gunsUsed.push_back(std::tuple{gun, ammo, rounds});
    return true;
}
*/