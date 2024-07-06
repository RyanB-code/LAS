#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>

class Command {
public:
    explicit Command (std::string setKey, std::string setDescription);
    virtual ~Command ();

    std::string getKey          () const;
    std::string getDescription  () const;
    
    virtual std::pair<int, std::ostringstream> execute () const;
private:
    std::string key, description;
};

using CommandPtr = std::unique_ptr<Command>;
using CommandList = std::vector<CommandPtr>;