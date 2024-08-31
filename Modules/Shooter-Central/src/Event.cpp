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
bool Event::addGun(GunPtr gun, std::vector<std::pair <uint64_t, const Ammo&>> roundsShot){
    if(!gun)
        return false;
    
    if(gunsUsed.contains(gun->getName()))
        return false;

    // Add to round counter for gun
    
    
    gunsUsed.try_emplace(gun->getName(), gun);
    return gunsUsed.contains(gun->getName());        // verify gun has been added successfully by checking and returning
}