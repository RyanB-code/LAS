#include <LAS/Commands.h>


namespace LAS{
    Command::Command(const std::string& setKey, const std::string& setDescription)
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

        
        ReturnStatus returnInvalidArg (const std::string& arg){
            ReturnStatus status { };
            status.code = -1;
            status.msg << "Invalid argument \"" << arg << "\"\n";
            return status;
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


