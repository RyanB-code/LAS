#include "Commands.h"

Command::Command(std::string setKey , std::string setDescription)
    :   key         { setKey },
        description {setDescription }
{

}
Command::~Command(){

}

std::string Command::getKey() const{
    return key;
}
std::string Command::getDescription() const {
    return description;
}

bool Command::execute() const {
    throw std::runtime_error{"Invalid command execution state."};
    return false;
}



