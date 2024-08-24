#include "CommandsInternal.h"

using namespace LAS;
using namespace LAS::Commands;
using namespace LAS::CommandHelper;

namespace LAS::Commands{



Manual::Manual(std::weak_ptr<Shell> setShell)
    :   Command {"man", "Show manual pages"},
        shell   {setShell}
{

}
Manual::~Manual(){

}
std::pair<int, std::ostringstream> Manual::execute(const StringVector&){
    std::shared_ptr<Shell> tempShell { shell.lock() };

    if(!tempShell)
        return pairErrorWithMessage("\tCould not access necessary shell\n");

    std::ostringstream os;
    tempShell->getAllGroupsManuals(os);

    return pair(0, os.str());
}
// MARK: Set
Set::Set(   std::weak_ptr<DisplayManager> setDM,
            std::weak_ptr<ModuleManager>  setMM,
            std::weak_ptr<Logger>         setLogger) 
        :   Command {"set",     "<setting> <value>\n"
                                "--log-tag-text-box-size <int>\tSets the text box size for log tags\n"
                                "--log-msg-text-box-size <int>\tSets the text box size for log messages\n"
                                "--show-log-time <bool>   \t\tToggle showing of log times\n"
                                "--show-log-tag  <bool>   \t\tToggle showing of log tags\n"
                                "--show-log-msg  <bool>   \t\tToggle showing of log messages\n"
                                "--show-log-location <bool>   \t\tToggle showing of log location\n"
                    },
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
        return pair(-1, "\tCould not access necessary items\n");
    
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
                return pairErrorWithMessage("Size cannot be below 5");

            if(size > 200 )
                return pairErrorWithMessage("Size cannot be greater than 200");
            
            auto settings = tempLogger->getLogSettings();
            settings->textBoxWidth_tag = size;
            return pairNormal();
        }
        catch(std::exception& e){
            return pairErrorWithMessage(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "--log-msg-text-box-size"){
        try{
            int size {std::stoi(args[1])};
            if(size < 30 )
                return pairErrorWithMessage("Size cannot be below 30");

            if(size > 255 )
                return pairErrorWithMessage("Size cannot be greater than 255");
            
            auto settings = tempLogger->getLogSettings();
            settings->textBoxWidth_msg = size;
            return pairNormal();
        }
        catch(std::exception& e){
            return pairInvalidArgument(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "--show-log-time"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTime = true;
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTime = false;
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-tag"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTags = true;
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showTags = false;
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-msg"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showMsg = true;
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showMsg = false;
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "--show-log-location"){
        if(stringValueTrue(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showLocation = true;
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = tempLogger->getLogSettings();
            settings->showLocation = false;
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else{
        return pairInvalidArgument(args[0]);
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
        return pairErrorWithMessage("\tCould not access necessary items\n");
    
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
                pairInvalidArgument(option);
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
                "\tModule Directory: " << tempModuleManager->getModuleLoadDirectory() << "\n";
    }
    if(addDisplaySettings)
        os <<   "Display Settings:\n"
                "\tNothing to display...\n";

    return pair(0, os.str());
}

// MARK: Echo
Echo::Echo() : Command {"echo", "Echos to all outputs"} 
{

}
Echo::~Echo(){

}
std::pair<int, std::ostringstream> Echo::execute(const StringVector& args) {
    std::ostringstream text;

    if(args.empty()){
        return pair(0, "\n");
    }
    
    for(std::string s : args){
        text << s << " ";
    }

    text << "\n";

    return pair(0, text.str());
}

// MARK: Module Control
ModuleControl::ModuleControl(   std::weak_ptr<DisplayManager>   setDisplayManager,
                                std::weak_ptr<ModuleManager>    setModuleManager
                            )
    :   Command {"modulectl",   "Controls module functions\n"
                                "<action> <command> [optional]\n"
                                "set\t\t\tApplies changes\n" 
                                "\t\tmodule-load-directory [option] <directory>\tChange directory where modules are loaded from\n\t\t\tOptional: -c creates directory specified\n"               
                },
                                displayManager  {setDisplayManager},
                                moduleManager   {setModuleManager}
{

}
ModuleControl::~ModuleControl(){

}
std::pair<int, std::ostringstream> ModuleControl::execute(const StringVector& args){
    std::shared_ptr<DisplayManager> tempDisplayManager { displayManager.lock()};
    std::shared_ptr<ModuleManager>  tempModuleManager  { moduleManager.lock()};

    if(!tempDisplayManager|| !tempModuleManager)
        return pairErrorWithMessage("\tCould not access necessary items\n");

    // 0 = verb, 1 = what, 2 = option/dir, 3 = directory
    
    bool setDirectory       {false};
    bool createDirectory    { false };
    bool reload             { false };  

    // Parse and handle arguments
    if(args[0] == "set"){  
        if(args[1] == "module-load-directory")
            setDirectory = true;
    }
    else if (args[0] == "reload-all"){
        reload = true;
    }
    else
        return pairInvalidArgument(args[0]);


    // Conditional actions
    if(setDirectory){
        // Check for optional
        std::string arg2 {args[2]};
        if(arg2.starts_with('-')){
            if(arg2.contains('c'))
                createDirectory = true;
            if(arg2.contains('r'))
                reload = true;
        }
        
        if(createDirectory && args.size() >= 4){
            if(!std::filesystem::exists(args[3])){
                LAS::TextManip::ensureSlash(args[3]);

                // Attempt to create
                if(!std::filesystem::create_directory(args[3]))
                   return pairErrorWithMessage("Could not create directory [" + args[3] + "]\nProcess aborted\n");
            }

            // Set directory
            if(!tempModuleManager->setModuleLoadDirectory(args[3]))
                return pairErrorWithMessage("Failed to set module directory to \"" + args[3] + "\"\nProcess aborted\n");
        }
        else{
            // No optional found
            if(!tempModuleManager->setModuleLoadDirectory(args[3]))
                return pairErrorWithMessage("Failed to set module directory to \"" + args[3] + "\"\n");
        }
    }

    if(reload){
        tempDisplayManager->closeAllModuleWindows();
        tempDisplayManager->clearAllModuleWindows();
        tempModuleManager->clearNonUtilityModules();

        // Load Modules
        StringVector notLoaded{};
        tempModuleManager->loadAllModules(*ImGui::GetCurrentContext(), notLoaded);

        // Load Windows
        int windowsNotLoaded{0};
        for(auto window : tempModuleManager->getAllWindows()){
            if(!tempDisplayManager->addWindow(window))
                ++windowsNotLoaded;
        }

        // Log Modules loaded
        std::ostringstream msg;
        if(!notLoaded.empty()){
            msg << "There were [" << notLoaded.size() << "] modules that could not be loaded.\nLocations: ";
            for(const auto& s : notLoaded){
                msg << "[" << s << "], ";
            }
            msg << '\n';
        }
        else
            msg << "All modules were successfully loaded\n";

        // Log Windows loaded
        if(windowsNotLoaded > 0){
            std::ostringstream msg;
            msg << "There were [" << windowsNotLoaded << "] windows that could not be loaded from modules\n";
        }
        else
            msg << "All windows successfully loaded from all modules\n";

        return pair(0, msg.str());
    }

    return pairErrorWithMessage("Ill formed command\n");
}
// MARK: Information
Information::Information() : Command {"info", "Displays build information.\n"}
{

}
Information::~Information(){
    
}
std::pair<int, std::ostringstream> Information::execute(const StringVector&) {
    std::ostringstream os;
    os << LAS::Environment::getInformation();
    os << "LAS-SDK version " << LAS::SDK::getVersion() << "\n";
    return pair(0, os.str());
}




} // end of LAS::Commands