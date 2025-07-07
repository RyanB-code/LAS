#include "CommandsInternal.h"

using namespace LAS;
using namespace LAS::Commands;
using namespace LAS::CommandHelper;

namespace LAS::Commands{



Manual::Manual(std::shared_ptr<Shell> setShell)
    :   Command {"man", "Show manual pages"},
        shell   {setShell}
{

}
Manual::~Manual(){

}
std::pair<int, std::ostringstream> Manual::execute(const StringVector&){
    std::ostringstream os;
    shell->getAllGroupsManuals(os);

    return pair(0, os.str());
}
// MARK: Set
Set::Set(   std::shared_ptr<DisplayManager> setDM, std::shared_ptr<ModuleManager>  setMM, std::shared_ptr<Shell> setShell 
        ) 
        :   Command {"set",     "<setting> <value>\n"
                                "log-tag-text-box-size <int>\tSets the text box size for log tags\n"
                                "log-msg-text-box-size <int>\tSets the text box size for log messages\n"
                                "show-log-time <bool>   \t\tToggle showing of log times\n"
                                "show-log-tag  <bool>   \t\tToggle showing of log tags\n"
                                "show-log-msg  <bool>   \t\tToggle showing of log messages\n"
                                "show-log-location <bool>   \tToggle showing of log location\n"
                                "rc-file-path <path>    \t\tSet desired RC file for the LAS Environment. It will not create file in path specified if not found\n"
                    },
            displayManager  {setDM},
            moduleManager   {setMM},
            shell           {setShell}
{

}
Set::~Set(){

}
std::pair<int, std::ostringstream> Set::execute(const StringVector& args) {
    std::pair<int, std::ostringstream> returnBuf;

    if(args.size() < 2){
        returnBuf.first = -1;
        returnBuf.second << "\tThis command requires two arguments.\n";
        return returnBuf;
    }

    if(args[0] == "log-tag-text-box-size"){
        try{
            int size {std::stoi(args[1])};
            if(size < 5 )
                return pairErrorWithMessage("Log Tag size cannot be below 5");

            if(size > 200 )
                return pairErrorWithMessage("Log Tag size cannot be greater than 200");
            
            LogSettings settings = Logging::getGlobalSettings();
            settings.textBoxWidth_tag = size;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        catch(std::exception& e){
            return pairErrorWithMessage(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "log-msg-text-box-size"){
        try{
            int size {std::stoi(args[1])};
            if(size < 20 )
                return pairErrorWithMessage("Log Message size cannot be below 30");

            if(size > 255 )
                return pairErrorWithMessage("Log Message size cannot be greater than 255");
            
            auto settings = Logging::getGlobalSettings();
            settings.textBoxWidth_msg = size;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        catch(std::exception& e){
            return pairInvalidArgument(args[1]);
        }
        return returnBuf;
    }
    else if(args[0] == "show-log-time"){
        if(stringValueTrue(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showTime = true;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showTime = false;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "show-log-tag"){
        if(stringValueTrue(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showTags = true;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showTags = false;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "show-log-msg"){
        if(stringValueTrue(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showMsg = true;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showMsg = false;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "show-log-location"){
        if(stringValueTrue(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showLocation = false;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else if(stringValueFalse(args[1])){
            auto settings = Logging::getGlobalSettings();
            settings.showLocation = false;
            Logging::setGlobalSettings(settings);
            return pairNormal();
        }
        else
            return pairInvalidArgument(args[1]);
    }
    else if(args[0] == "rc-file-path"){
        if(shell->setRCPath(args[1], false))
            return pairNormal();
        else
            return pairErrorWithMessage("Could not set RC file path to \"" + args[1] + "\"\n");
    }   
    else{
        return pairInvalidArgument(args[0]);
    }

    return pairInvalidArgument(args[0]);

}

// MARK: Print
Print::Print(   std::shared_ptr<DisplayManager> setDM, std::shared_ptr<ModuleManager>  setMM )
        :   Command {"print",   "Prints information\n"
                                "<arg> [option]\n"
                                "\t--settings [option]\t Show all settings\n"
                                "\t             -l    \t Show log settings\n"
                                "\t             -m    \t Show module settings\n"
                                "\t             -d    \t Show display settings\n"
                    },
            displayManager  {setDM},
            moduleManager   {setMM}
{

}
Print::~Print(){

}
std::pair<int, std::ostringstream> Print::execute(const StringVector& args) {
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
    else
        return pairInvalidArgument(args[0]); 


    if(addLogSettings){
        LogSettings settings {Logging::getGlobalSettings()};
        os <<   "Log Settings:\n"
                "\tShow Time:       \t"         << std::boolalpha << settings.showTime <<  "\n"
                "\tShow Tags:       \t"         << std::boolalpha << settings.showTags <<  "\n"
                "\tShow Message:    \t"         << std::boolalpha << settings.showMsg <<  "\n"
                "\tShow Location:   \t"         << std::boolalpha << settings.showLocation <<  "\n"
                "\tTag Text Box Size:     "     << (int)settings.textBoxWidth_tag <<  "\n"
                "\tMessage Text Box Size: "     << (int)settings.textBoxWidth_msg <<  "\n";
    }
    if(addModuleSettings){
        os <<   "Module Manager Settings:\n"  
                "\tModule Directory: " << moduleManager->getModuleLoadDirectory() << "\n";
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
ModuleControl::ModuleControl(   std::shared_ptr<DisplayManager>   setDisplayManager, std::shared_ptr<ModuleManager> setModuleManager, std::shared_ptr<Shell> setShell )
    :   Command {"modulectl",   "Interact with the module controller\n"
                                "<action> <command> [optional]\n"
                                "set <arg>\t\t\tApplies changes\n" 
                                "\t\tmodule-load-directory [option] <directory>\tChange directory where modules are loaded from\n\t\t\tOptional: -c creates directory specified\n"
                                "list-modules\t\tLists all modules by name\n"
                                "reload-all  \t\tReloads all modules from file\n"
                                "reload <name> \tRe-runs a module's RC file\n"
                },
                                displayManager  {setDisplayManager},
                                moduleManager   {setModuleManager},
                                shell           {setShell}
{

}
ModuleControl::~ModuleControl(){

}
std::pair<int, std::ostringstream> ModuleControl::execute(const StringVector& args){
    // 0 = verb, 1 = what, 2 = option/dir, 3 = directory
    
    bool setDirectory       { false };
    bool createDirectory    { false };
    bool reload             { false };  
    bool listModules        { false };
    bool reloadByName       { false };

    // Parse and handle arguments
    if(args.empty())
        return pairErrorWithMessage("This command requires at least one argument\n");
    else if(args[0] == "set"){  
        if(args[1] == "module-load-directory")
            setDirectory = true;
    }
    else if (args[0] == "reload-all")
        reload = true;
    else if (args[0] == "reload" && args.size() > 1)
        reloadByName = true;
    else if (args[0] == "list-modules")
        listModules = true;
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
            if(!moduleManager->setModuleLoadDirectory(args[3]))
                return pairErrorWithMessage("Failed to set module directory to \"" + args[3] + "\"\nProcess aborted\n");
        }
        else{
            // No optional found
            if(!moduleManager->setModuleLoadDirectory(args[3]))
                return pairErrorWithMessage("Failed to set module directory to \"" + args[3] + "\"\n");
        }
    }

    if(reload){
        displayManager->closeAllWindows();
        displayManager->clearModuleWindows();
        moduleManager->clearModules();

        // Load Modules
        StringVector notLoaded{};
        moduleManager->loadAllModules(*ImGui::GetCurrentContext(), notLoaded);

        // Load Windows
        int windowsNotLoaded{0};
        for(auto name : moduleManager->getModuleNames()){
            auto mod { moduleManager->getModule(name) };
            if(!displayManager->addWindow(mod->getModuleInfo().title, std::function<void()>{mod->getModuleInfo().drawFunction}))
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

    if(reloadByName){
        if(!moduleManager->containsModule(args[1]))
            return pairErrorWithMessage("Could not find module \"" + args[1] + "\"\n");

        std::string moduleRCFilePath {moduleManager->getModule(args[1])->getRCFilePath()};
        std::queue<std::string> lines;

        LAS::ShellHelper::readRCFile(moduleRCFilePath, lines);          // Read RC file 
        for (/*Nothing*/; !lines.empty(); lines.pop())                  // Add to command queue
            shell->addToQueue(lines.front());
        return pairNormal();        
    }

    if(listModules){
        std::ostringstream os;
        for(auto& s : moduleManager->getModuleNames()){
            os << s << "\n";
        }
        return pair(0, os.str());
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

// MARK: Reload
Reload::Reload(std::shared_ptr<Shell> setShell)
    :   Command {"reload", "Runs the Environment's .las-rc file\n"},
        shell   {setShell}
{

}
Reload::~Reload(){

}
std::pair<int, std::ostringstream> Reload::execute(const StringVector& args){
    // Reset all aliases
    shell->removeAllAliases();

    if(!shell->readRCFile())
        return pairErrorWithMessage("\tFailure to read RC file at \"" + shell->getRCPath() + "\"\n");
    else
        return pairNormal();

}
// MARK: Display Control
DisplayControl::DisplayControl(std::shared_ptr<DisplayManager> setDisplayManager ) 
    :   Command         { "displayctl", "Interact with the display controller\n"
                                "save-config          \tSaves the window configuration\n"
                                "show-all-window-info \tShows all window information\n"
                                "<title> <command>\n"
                                "\t\topen   \tOpens the window\n"
                                "\t\tclose  \tCloses the window\n"
                        },
        displayManager  { setDisplayManager }
{

}
DisplayControl::~DisplayControl(){

}
std::pair<int, std::ostringstream> DisplayControl::execute(const StringVector& args){
    if(args.empty())
        return pairErrorWithMessage("This command requires at least one argument\n");
    else if(args[0] == "save-config"){
        if(displayManager->saveWindowConfig())
            return pairNormal();
        else
            return pairErrorWithMessage("Could not save window configuration\n");
    }
    else if(args[0] == "show-all-window-info"){
        constexpr uint8_t idWidth   { 4 };
        constexpr uint8_t nameWidth { 30 };
        constexpr uint8_t openWidth { 7 };

        uint64_t 	tableWidth 	{ idWidth + nameWidth + openWidth };

        std::ostringstream msg;
        msg << std::format("{:<{}}", "Name",	nameWidth);
        msg << std::format("{:<{}}", "Status",	openWidth);
        msg << '\n' << std::format("{:-<{}}", '-', tableWidth) << "\n";

        for(auto itr {displayManager->cbegin()}; itr != displayManager->cend(); ++itr){
            msg << std::format("{:<{}}", itr->second.title, nameWidth);

            std::string shownString;
            bool open {displayManager->shown(itr->second.title)};
            open ? shownString = "open" : shownString = "closed";
            msg << std::format("{:<{}}", shownString, openWidth);
            msg << '\n';
        }        
        return pair(0, msg.str());
    }


    if(args.size() == 2){
        if(!displayManager->containsWindow(args[0]))
            return pairErrorWithMessage("No window was found with ID " + args[0]);

        bool shownStatus { false };
        if(args[1] == "open")
            shownStatus = true;
        else if(args[1] == "close")
            shownStatus = false;
        else
            return pairInvalidArgument(args[1]);

        bool& open {*displayManager->shown(args[0])};
        open = shownStatus;
        
        return pairNormal();
    }

    return pairErrorWithMessage("Ill formed command\n");

}


} // end of LAS::Commands
