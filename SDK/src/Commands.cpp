#include <LAS/Commands.h>


namespace LAS{
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
    namespace CommandHelper{

        std::pair<int, std::ostringstream> pair(int value, const std::string& msg){
            std::pair<int, std::ostringstream> returnBuf;
            returnBuf.first = value;
            returnBuf.second << LAS::TextManip::ensureNewline(msg);
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
            returnBuf.second << "Invalid argument \"" << arg << "\"\n";

            return returnBuf;
        }
        std::pair<int, std::ostringstream> pairErrorWithMessage(const std::string& msg){
            std::pair<int, std::ostringstream> returnBuf;
            returnBuf.first = -1;
            returnBuf.second << LAS::TextManip::ensureNewline(msg);
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
}


