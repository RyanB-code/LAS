#include "CommandsInternal.h"

using namespace LAS::Commands;

// MARK: TEST COMMAND
TestCommand::TestCommand() : Command {"test", "Testing command"}
{

}

TestCommand::~TestCommand(){

}

std::pair<int, std::ostringstream> TestCommand::execute(const StringVector& args) {
    std::pair<int, std::ostringstream> returnVal;
    returnVal.first = 0;
    returnVal.second << "Test Command:\n";
    for(const auto& s : args){
        returnVal.second << "\t" << s << "\n";
    }

    return returnVal;    
}

// MARK: Set
Set::Set(   const std::weak_ptr<DisplayManager> setDM,
            const std::weak_ptr<ModuleManager>  setMM,
            const std::weak_ptr<Logger>         setLogger) 
        :   Command {"set", "Description Here"},
            displayManager  {setDM},
            moduleManager   {setMM},
            logger          {setLogger}
{

}
Set::~Set(){

}
std::pair<int, std::ostringstream> Set::execute(const StringVector& args) {
    std::shared_ptr<DisplayManager> temp_displayManager { displayManager.lock()};
    std::shared_ptr<ModuleManager>  temp_moduleManager  { moduleManager.lock()};
    std::shared_ptr<Logger>         temp_logger         { logger.lock()};

    if(!temp_displayManager|| !temp_moduleManager || !temp_logger)
        return std::pair<int, std::ostringstream>(-1, "\tCould not access necessary items\n");
    
    std::pair<int, std::ostringstream> returnBuf;

    if(args.size() < 2){
        returnBuf.first = -1;
        returnBuf.second << "\tThis command requires two arguments.\n";
        return returnBuf;
    }

    if(args[0] == "--log-tag-text-box-size"){
        try{
            int size {std::stoi(args[1])};
            if(size < 5 )
                return returnErrorWithMessage("Size cannot be below 5");

            if(size > 200 )
                return returnErrorWithMessage("Size cannot be greater than 200");
            
            auto settings = temp_logger->getLogSettings();
            settings->textBoxWidth_tag = size;
            return returnNormal();
        }
        catch(std::exception& e){
            return returnInvalidArgument(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "--log-msg-text-box-size"){
        try{
            int size {std::stoi(args[1])};
            if(size < 30 )
                return returnErrorWithMessage("Size cannot be below 30");

            if(size > 255 )
                return returnErrorWithMessage("Size cannot be greater than 255");
            
            auto settings = temp_logger->getLogSettings();
            settings->textBoxWidth_msg = size;
            return returnNormal();
        }
        catch(std::exception& e){
            return returnInvalidArgument(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "--show-log-time"){
        if(stringValueTrue(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showTime = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showTime = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-tag"){
        if(stringValueTrue(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showTags = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showTags = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-msg"){
        if(stringValueTrue(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showMsg = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showMsg = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-location"){
        if(stringValueTrue(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showLocation = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = temp_logger->getLogSettings();
            settings->showLocation = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else{
        return returnInvalidArgument(args[0]);
    }
}