#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <cstring>

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

using CommandPtr = std::unique_ptr<Command>;
using CommandList = std::vector<CommandPtr>;

namespace LAS{
    std::pair<int, std::ostringstream> returnInvalidArgument(const std::string& arg);
    std::pair<int, std::ostringstream> returnNormal();
    std::pair<int, std::ostringstream> returnErrorWithMessage(const std::string& msg);

    bool stringValueTrue    (std::string s);
    bool stringValueFalse   (std::string s);

}