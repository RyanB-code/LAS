#pragma once

#include <vector>
#include <sstream>

namespace LAS{
    
    struct ReturnStatus {
        int code { 0 };
        std::ostringstream msg { };
    };
    class Command {
    public:
        explicit Command (const std::string& setKey, const std::string& setDescription);
        virtual ~Command ();

        std::string getKey          () const;
        std::string getDescription  () const;
        
        virtual ReturnStatus execute (const std::vector<std::string>& args) = 0;
    private:
        std::string key, description;
    };

    namespace CommandHelper{
        ReturnStatus returnInvalidArg (const std::string& arg);

        bool stringValueTrue    (std::string s);
        bool stringValueFalse   (std::string s);
    }
}
