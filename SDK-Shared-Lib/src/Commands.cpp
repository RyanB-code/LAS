#include <LAS/Commands.h>

using namespace LAS;

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

namespace LAS{
    std::pair<int, std::ostringstream> pair(int value, const std::string& msg){
        std::pair<int, std::ostringstream> returnBuf;
        returnBuf.first = value;
        returnBuf.second << msg;
        return returnBuf;
    }
    std::pair<int, std::ostringstream> pairNormal(){
        std::pair<int, std::ostringstream> returnBuf;
        returnBuf.first = 0;
        returnBuf.second << "";
        return returnBuf;
    }
    std::pair<int, std::ostringstream> pairInvalidArgument(const std::string& arg){
        std::pair<int, std::ostringstream> returnBuf;
        returnBuf.first = -1;
        returnBuf.second << "\tInvalid argument \"" << arg << "\"\n";

        return returnBuf;
    }
    std::pair<int, std::ostringstream> pairErrorWithMessage(const std::string& msg){
        std::pair<int, std::ostringstream> returnBuf;
        returnBuf.first = -1;
        returnBuf.second << "\t" << msg << "\n";
        return returnBuf;
    }

    bool stringValueTrue    (std::string s){
        for(auto& c : s)
            c = std::tolower(c);

        if(s == "true")
            return true;
        else
            return false;
    }
    bool stringValueFalse   (std::string s){
        for(auto& c : s)
            c = std::tolower(c);

        if(s == "false")
            return true;
        else
            return false;
    }
}


