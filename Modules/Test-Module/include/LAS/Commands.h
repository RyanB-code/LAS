#pragma once

#include "HelperFunctions.h"

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <cstring>

namespace LAS{
    class Command {
    public:
        explicit Command (std::string setKey, std::string setDescription);
        virtual ~Command ();

        std::string getKey          () const;
        std::string getDescription  () const;
        
        virtual std::pair<int, std::ostringstream> execute (const std::vector<std::string>& args) = 0;
    private:
        std::string key, description;
    };

    using CommandPtr    = std::unique_ptr<Command>;
    using CommandList   = std::vector<CommandPtr>;

    namespace CommandHelper{
        std::pair<int, std::ostringstream> pair                     (int returnVal, const std::string& msg);
        std::pair<int, std::ostringstream> pairNormal               ();
        std::pair<int, std::ostringstream> pairErrorWithMessage     (const std::string& msg);
        std::pair<int, std::ostringstream> pairInvalidArgument      (const std::string& arg);

        bool stringValueTrue    (std::string s);
        bool stringValueFalse   (std::string s);
    }
}