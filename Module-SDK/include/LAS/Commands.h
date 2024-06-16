#pragma once

#include <string>
#include <memory>
#include <vector>

class Command {
public:
    explicit Command (std::string setKey, std::string setDescription);
    ~Command ();

    std::string getKey          () const;
    std::string getDescription  () const;
    
    virtual bool execute () const;
private:
    std::string key, description;
};

using CommandPtr = std::unique_ptr<Command>;
using CommandList = std::vector<CommandPtr>;