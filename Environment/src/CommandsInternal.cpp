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

Manual::Manual(std::weak_ptr<Shell> setShell)
    :   Command {"man", "Show manual pages\n"},
        shell   {setShell}
{

}
Manual::~Manual(){

}
std::pair<int, std::ostringstream> Manual::execute(const StringVector&){
    std::shared_ptr<Shell> tempShell { shell.lock() };

    if(!tempShell)
        return returnPair(-1, "\tCould not access necessary shell\n");

    std::ostringstream os;
    
    // Buffer the output of the description accordingly to add padding to newlines
    const std::string padding {"                                "};     // 32 spaces is the size of 2 tabs and 20 chars

    os << "LAS Manual Page\n";

    for(const auto& command : tempShell->viewCommandInfo()){
        std::string description {command.second->getDescription()};
        size_t charNum { 0 };

        while(charNum < description.size()){
            char& c {description[charNum]};
            if(c == '\n'){
                description.insert(charNum+1, padding);
                charNum += padding.size();
            }
            ++charNum;
        }

        // Display output
        os << std::format("\t{:20}\t", command.second->getKey());
        os << std::format("\t{}\n", description);
    }

    return returnPair(0, os.str());
}
// MARK: Set
Set::Set(   std::weak_ptr<DisplayManager> setDM,
            std::weak_ptr<ModuleManager>  setMM,
            std::weak_ptr<Logger>         setLogger) 
        :   Command {"set",     "<setting> <value>\n"
                                "--log-tag-text-box-size <int>\tSets the text box size for log tags\n"
                                "--log-msg-text-box-size <int>\tSets the text box size for log messages\n"
                                "--show-log-time <bool>   \t\tToggle showing of log times\n"
                                "--module-directory [optional] <directory>\tChange directory where modules are loaded from\n\tOptional: -c creates directory specified"},
            displayManager  {setDM},
            moduleManager   {setMM},
            logger          {setLogger}
{

}
Set::~Set(){

}
std::pair<int, std::ostringstream> Set::execute(const StringVector& args) {
    std::shared_ptr<DisplayManager> tempDisplayManager { displayManager.lock()};
    std::shared_ptr<ModuleManager>  tempModuleManager  { moduleManager.lock()};
    std::shared_ptr<Logger>         tempLogger         { logger.lock()};

    if(!tempDisplayManager|| !tempModuleManager || !tempLogger)
        return returnPair(-1, "\tCould not access necessary items\n");
    
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
            
            auto settings = tempLogger->getLogSettings();
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
            
            auto settings = tempLogger->getLogSettings();
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
            auto settings = tempLogger->getLogSettings();
            settings->showTime = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTime = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-tag"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTags = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTags = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-msg"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showMsg = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showMsg = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-location"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showLocation = true;
            return returnNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showLocation = false;
            return returnNormal();
        }
        else
            return returnInvalidArgument(args[1]);
    }
    else if(args[0] == "--module-directory"){
        bool setDirectory {false};

        // Option to create directory
        if(args.size() >= 3){
            if(args[1] == "-c"){
                if(std::filesystem::exists(args[2]))
                    setDirectory = true;
                else{
                    LAS::TextManip::ensureSlash(args[2]);
                    if(std::filesystem::create_directory(args[2]))
                        setDirectory = true;
                }
            }
            else
                return returnInvalidArgument(args[1]);
        }
        else if(args.size() == 2){
            setDirectory = true;
        }


        if(setDirectory){
            if(tempModuleManager->setModuleDirectory(args[2]))
                return returnNormal();
            else
                return returnErrorWithMessage("Failed to set module directory to \"" + args[2] + "\"\n");
        }

        return returnErrorWithMessage("Failed to set module directory\n");
    }
    else{
        return returnInvalidArgument(args[0]);
    }
}

// MARK: Print
Print::Print(   std::weak_ptr<DisplayManager> setDM,
                std::weak_ptr<ModuleManager>  setMM,
                std::weak_ptr<Logger>         setLogger) 
        :   Command {"print",   "Prints information\n"
                                "<arg> [option]\n"
                                "\t--settings [option]\t Show all settings\n"
                                "\t             -l    \t Show log settings\n"
                                "\t             -m    \t Show module settings\n"
                                "\t             -d    \t Show display settings\n"
                    },
            displayManager  {setDM},
            moduleManager   {setMM},
            logger          {setLogger}
{

}
Print::~Print(){

}
std::pair<int, std::ostringstream> Print::execute(const StringVector& args) {
    std::shared_ptr<DisplayManager> tempDisplayManager { displayManager.lock()};
    std::shared_ptr<ModuleManager>  tempModuleManager  { moduleManager.lock()};
    std::shared_ptr<Logger>         tempLogger         { logger.lock()};

    if(!tempDisplayManager|| !tempModuleManager || !tempLogger)
        return returnPair(-1, "\tCould not access necessary items\n");
    
    std::ostringstream os;

    bool addLogSettings {false}, addModuleSettings{false}, addDisplaySettings{false}, optionFound{false};

    if(args[0] == "--settings"){
        if(args.size() == 2){
            std::string option {args[1]}; // Gets the optionals

            if(option.starts_with('-') && option.contains('l')){
                addLogSettings  = true;
                optionFound     = true;
            }

            if(option.starts_with('-') && option.contains('m')){
                addModuleSettings   = true;
                optionFound         = true;
            }

            if(option.starts_with('-') && option.contains('d')){
                addDisplaySettings  = true;
                optionFound         = true;
            }

            if(!option.starts_with('-') || !optionFound){
                returnInvalidArgument(option);
            }
        }
        else{
            addLogSettings      = true;
            addModuleSettings   = true;
            addDisplaySettings  = true;
        }
    }


    if(addLogSettings){
        const LogSettingsPtr settings {tempLogger->getLogSettings()};
        os <<   "Log Settings:\n"
                "\tShow Time:       \t"         << std::boolalpha << settings->showTime <<  "\n"
                "\tShow Tags:       \t"         << std::boolalpha << settings->showTags <<  "\n"
                "\tShow Message:    \t"         << std::boolalpha << settings->showMsg <<  "\n"
                "\tShow Location:   \t"         << std::boolalpha << settings->showLocation <<  "\n"
                "\tTag Text Box Size:     "     << (int)settings->textBoxWidth_tag <<  "\n"
                "\tMessage Text Box Size: "     << (int)settings->textBoxWidth_msg <<  "\n";
    }
    if(addModuleSettings){
        os <<   "Module Manager Settings:\n"  
                "\tModule Directory: " << tempModuleManager->getModuleDirectory() << "\n";
    }
    if(addDisplaySettings)
        os <<   "Display Settings:\n"
                "\tNothing to display...\n";

    return returnPair(0, os.str());
}
