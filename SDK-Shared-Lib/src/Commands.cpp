#include <LAS/Commands.h>

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

std::pair<int, std::ostringstream> Command::execute() const {
    throw std::runtime_error{"Invalid command execution state."};
    std::pair<int, std::ostringstream> returnVal;
    returnVal.first = 0;
    returnVal.second << "Invalid command execution state.";
    return returnVal;
}



