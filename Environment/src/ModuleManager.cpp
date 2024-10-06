#include "ModuleManager.h"

using namespace LAS;

// MARK: TEST
ModuleMessageSubscriberTest::ModuleMessageSubscriberTest()
{

}
ModuleMessageSubscriberTest::~ModuleMessageSubscriberTest(){

}
void ModuleMessageSubscriberTest::notifiy() {
    std::cout << "Notified\n";
}
// MARK: MOD MSG SUB
ModuleMessageSubscriber::ModuleMessageSubscriber(){
    static uint8_t IDs { 0 };
    ++IDs;
    ID = IDs;
}
ModuleMessageSubscriber::~ModuleMessageSubscriber(){

}
uint8_t ModuleMessageSubscriber::getID() const{
    return ID;
}

// MARK: MODULE MSG MAN
ModuleMessageManager::ModuleMessageManager(){

}
ModuleMessageManager::~ModuleMessageManager(){
    
}
void ModuleMessageManager::addMsg(std::string msg) {
    if(subscribers.empty())
        return;

    auto& element { messageQueue.emplace_back(std::pair{msg, std::vector<uint8_t>{} } )};
    
    std::vector<uint8_t> IDs { };
    for(const auto& subscriber : subscribers){
        IDs.emplace_back( subscriber->getID() );
    }

    for(const auto& ID : IDs)
        element.second.emplace_back(ID);

    for(const auto& subscriber : subscribers)
        if(subscriber)
            subscriber->notifiy();
}
bool ModuleMessageManager::addSubscriber(ModMsgSubPtr subscriber){
    subscribers.emplace_back(subscriber);
    return true;
}
void ModuleMessageManager::retrieveMessages(uint8_t ID, std::queue<std::string>& messages){
    std::vector<std::pair<std::string, std::vector<uint8_t>>>::iterator itr;

    std::vector<std::vector<std::pair<std::string, std::vector<uint8_t>>>::iterator> entriesToErase;

    for(itr = messageQueue.begin(); itr != messageQueue.end(); ++itr ){
        auto& pair {*itr};

        std::vector<uint8_t>::iterator idItr;
        std::vector<std::vector<uint8_t>::iterator> idEntriesToErase;
        for(idItr = pair.second.begin(); idItr != pair.second.end(); ++idItr){
            if(*idItr == ID){
                messages.push(pair.first);  // If the ID is found in the map, that means the MSG was not sent, therefore add to messages
                idEntriesToErase.emplace_back(idItr);              // Remove ID from MAP meaning message was given
            }
        }

        // Erase ID from pair
        for(const auto& idItr : idEntriesToErase)
            pair.second.erase(idItr);


        // If a map is empty, that means it can be erased from the queue
        if(pair.second.empty())
            entriesToErase.emplace_back(itr);
    }

    // Erase the empty queues
    for(const auto& eraseItr : entriesToErase)
        messageQueue.erase(eraseItr);
}
void ModuleMessageManager::viewMessages (std::ostringstream& os){
    constexpr uint8_t msgWidth  { 40 };
    constexpr uint8_t idWidth   { 40 };
    constexpr uint8_t openWidth { 7 };

    os << std::format("{:<{}}", "MSG",		msgWidth);
    os << std::format("{:<{}}", "IDs",	    idWidth);
    
    os << "\n";
    
    for(const auto& pair : messageQueue){
        os << std::format("{:<{}}", pair.first, msgWidth);

        std::ostringstream IDs;
        if(!pair.second.empty()){
            for(const auto& ID : pair.second)
                IDs << "ID: " << int{ID} << ". ";

            os << std::format("{:<{}}", IDs.str(), idWidth);
        }
        else
            os << std::format("{:<{}}", "IDs Empty", idWidth);

         os << "\n";
    }

    os << "\nEnd Msgs\n";
}


// MARK: MODULE MANAGER
ModuleManager::ModuleManager(const LoggerPtr& setLogger) :
    logger {setLogger}
{

}
ModuleManager::~ModuleManager(){

}
bool ModuleManager::addModule(const ModulePtr& module){
    // Check if modules contains a module of the same title before adding
    if(modules.contains(module->getTitle())){
        return false;
    }
    else{
        modules.try_emplace(module->getTitle(), module);
        return modules.contains(module->getTitle());        // verify module has been added successfully by checking and returning
    }
}
bool ModuleManager::removeModule(std::string title){
    if(modules.contains(title)){
        modules.at(title)->cleanup();
        modules.erase(title);
        return !modules.contains(title); // Return the inverse of contain() -> ie if modules still contains a member of the key (contains returns true), the return variable will be false since the erase did not work correctly
    }
    else{
        // Return true if the element already doesn't exist
        return true;
    }
}
ModulePtr ModuleManager::getModule(std::string title) const{
    if(modules.contains(title)){
        // No out_of_range exception should be thrown since I checked using contains()
        return modules.at(title);
    }
    else
        return nullptr;
}
bool ModuleManager::containsModule(std::string title) const{
    return modules.contains(title);
}
void ModuleManager::loadAllModules(ImGuiContext& context, StringVector& modulesNotLoaded, std::string loadDirectory, std::string filesDirectory){
    modulesNotLoaded.erase(modulesNotLoaded.begin(), modulesNotLoaded.end());

    if(loadDirectory.empty())
        loadDirectory = moduleLoadDirectory;
    if(filesDirectory.empty())
        filesDirectory = moduleFilesDirectory;
    
    const   std::filesystem::path   qualifiedDirectory  {LAS::TextManip::ensureSlash(loadDirectory)};   // Path with slashes

    if(!std::filesystem::exists(qualifiedDirectory)){
        throw std::filesystem::filesystem_error("Directory for modules does not exist", qualifiedDirectory, std::error_code());
    }

    // Iterate over directory and attempt to load each file
	for(auto const& file : std::filesystem::directory_iterator(qualifiedDirectory)){
        if(!loadModule(filesDirectory, context, file.path()))
            modulesNotLoaded.push_back(file.path());
	}

    return;
}
bool ModuleManager::loadModule  (std::string parentDirectory, ImGuiContext& context, const std::string& fileName){

    // Reject if not named correctly
    if(!fileName.ends_with(moduleNameSuffix)){
        logger->log("The file [" + fileName + "] was ignored for module loading. Improper name format.", Tags{"Non fatal", "Module Manager"});
        return false;
    }
    
    ModulePtr moduleBuffer;
    try{
        moduleBuffer = LAS::Modules::bindFiletoModule(fileName, logger, context);
    }
    catch(std::exception& e){
        logger->log(std::string{e.what()} + " from file [" + fileName + "]", Tags{"Non fatal", "Module Manager"});
        return false;
    }

    // This writes library information to the buffer
    if(!moduleBuffer->loadModuleInfo()){
        logger->log("loadModuleInfo() returned false from Module [" + moduleBuffer->getTitle() + "]. Loading procedure terminated", Tags{"Non fatal", "Module Manager"});
        return false;
    }

    // Verify module information
    std::ostringstream fatalMsg;
    switch(Modules::verifyModuleInformation(moduleBuffer)){
        case 0:
            // Success
            break;
         case 1:
            logger->log("Module buffer does not exist. From file [" + fileName + "]", Tags{"ERROR", "Module Manager"});
            return false;
            break;
        case 2:
            logger->log("Module title empty from file [" + fileName + "]", Tags{"ERROR", "Module Manager"});
            return false;
            break;
        case 3:
            logger->log("Command group is empty from Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
            return false;
            break;
        case 4:
            logger->log("Command group name rejected from Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
            return false;
            break;
        case 5:
            logger->log("Module SDK version and Environment SDK version mismatch from Module [" + moduleBuffer->getTitle() + "]. Issues may occur.", Tags{"WARNING", "Module Manager"});
            break;
        case 6:
            fatalMsg    << "Fatal version mismatch. Environment SDK is " << LAS::SDK::getVersion() 
                        << ". Module [" << moduleBuffer->getTitle() 
                        << "] SDK version is " << LAS::Information::versionToStr(moduleBuffer->getSDKVersion());
            logger->log( fatalMsg.str(), Tags{"ERROR", "Module Manager"});
            return false;
            break;
        default:
            logger->log("Failure to verify information for file [" + fileName + "]", Tags{"ERROR", "Module Manager"});
            return false;
            break;
    }

    std::string formattedTitle { LAS::TextManip::ensureAlNumNoSpaces(moduleBuffer->getTitle())};
    parentDirectory = LAS::TextManip::ensureSlash(parentDirectory);

    std::string rcPathTitle = {formattedTitle};

    // Makes rcPath all lower case
    for(auto& c : rcPathTitle){
        if(std::isalpha(c))
            c = std::tolower(c);
    }

    std::string moduleFilesDirectory    {LAS::TextManip::ensureSlash(parentDirectory) + LAS::TextManip::ensureSlash(formattedTitle)};
    std::string moduleRCFilePath        {moduleFilesDirectory + '.' + rcPathTitle + "-rc"};

    // Check directories can be made
    if(!LAS::ensureDirectory(moduleFilesDirectory)){
        logger->log("Could not find or create directory [" + moduleFilesDirectory + "] for Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }
    if(!LAS::ensureFile(moduleRCFilePath)){
        logger->log("Could not find or create file [" + moduleRCFilePath + "] for Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }

    // Pass environment info only if all files could be created
    EnvironmentInfo envInfo {moduleFilesDirectory, moduleRCFilePath, context, logger};
    if(!moduleBuffer->loadEnvInfo(envInfo)){
        logger->log("Failed loading environment info from Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }

    if(!addModule(moduleBuffer)){
        logger->log("Could not add module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }
    return true;
}

const StringVector ModuleManager::getModuleNames() const{
    StringVector names;
    for(auto module : modules){
        names.push_back(module.second->getTitle());
    }
    return names;
}
WindowList ModuleManager::getAllWindows()    const{
    WindowList list;

    for(auto module : modules){
        list.push_back(module.second->getWindow());
    }

    return list;
}
std::string ModuleManager::getModuleLoadDirectory() const{
    return moduleLoadDirectory;
}
bool ModuleManager::setModuleLoadDirectory(const std::string& directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);              // Ensure slash at the end

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    moduleLoadDirectory = qualifiedDirectory;
    return true;
}
std::string ModuleManager::getModuleFilesDirectory() const{
    return moduleFilesDirectory;
}
bool ModuleManager::setModuleFilesDirectory(const std::string& directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);              // Ensure slash at the end

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    moduleFilesDirectory = qualifiedDirectory;
    return true;
}
void ModuleManager::clearNonUtilityModules(){
    std::unordered_map<std::string, ModulePtr>::iterator itr;

    std::vector<std::unordered_map<std::string, ModulePtr>::iterator> modulesToErase{};

    for(itr = modules.begin(); itr != modules.end(); ++itr){
        if(itr->second->getWindow()->getMenuOption() != Windowing::MenuOption::UTILITY)
           modulesToErase.push_back(itr);
    }

    for(auto i : modulesToErase){
        i->second->cleanup();
        modules.erase(i);
    }
}
void ModuleManager::addMsg(const std::string& msg){
    messageManager.addMsg(msg);
}
void ModuleManager::viewMessages(std::ostringstream& os){
    messageManager.viewMessages(os);
}
bool ModuleManager::addSubscriber(ModMsgSubPtr subscriber){
    return messageManager.addSubscriber(subscriber);
}
void ModuleManager::retrieveMessages(uint8_t ID, std::queue<std::string>& messages){
    messageManager.retrieveMessages(ID, messages);
}

// MARK: LASCore Namespace 
namespace LAS::Modules{

    ModulePtr bindFiletoModule(const std::string& path, const LoggerPtr& logger, const ImGuiContext& context){
        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        if(!lib)
            throw std::runtime_error{"Failed to create handle from shared object file"};     // Do not continue if library could not be opened

        // Bind the API funcions
        LoadModuleInfo      loadModuleInfo    {reinterpret_cast<LoadModuleInfo>     (dlsym(lib, "LASM_loadModuleInfo"))};
        LoadEnvironmentInfo loadEnvInfo       {reinterpret_cast<LoadEnvironmentInfo>(dlsym(lib, "LASM_init"))};
        VoidNoParams        cleanup           {reinterpret_cast<VoidNoParams>       (dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding functions failed
        if(!loadModuleInfo || !loadEnvInfo ||!cleanup)
            throw std::runtime_error{"Failed to find necessary function signatures"};     // Do not continue if library could not be opened


        return std::make_shared<Module>(logger, loadModuleInfo, loadEnvInfo, cleanup);
    }
    int verifyModuleInformation(const ModulePtr& module) {
        if(!module)
            return 1;

        if(module->getTitle().empty())
            return 2;

        std::string groupName {module->getGroupName()};
        if(groupName.empty())
            return 3;
        
        for(const auto& c : groupName){
            if(!std::isalnum(c))
                if(c != '-')
                    return 4;
        }

        const Version sdkVersion    {LAS::SDK::getVersionMajor(), LAS::SDK::getVersionMinor(), LAS::SDK::getVersionPatch()};
        const Version moduleVersion {module->getSDKVersion()};
        std::ostringstream msg;

        switch (Modules::compareVersions(sdkVersion, moduleVersion)){
        case 0:
            return 0;
            break;
         case 1:
            return 5;
            break;
        case 2:
            return 6;
            break;
        default:
            return -1;
            break;
        }

        return true;
    }
    bool ensureModuleFiles (std::string moduleFilesDirectory, std::string moduleTitle){
        
        moduleFilesDirectory = LAS::TextManip::ensureSlash(moduleFilesDirectory);

        std::string moduleDirectory { moduleFilesDirectory + moduleTitle + '/'};
        std::string moduleRCFile    { moduleDirectory + '.' + moduleTitle + "-rc"};

        if(!LAS::ensureDirectory(moduleDirectory))
            return false;
        if(!LAS::ensureFile(moduleRCFile))
            return false;

        return true;
    }
    int compareVersions (const Version& base, const Version& compare){
        if(base.major != compare.major)
            return 2;

        if(base.minor >= compare.minor)
            return 0;
        
        if(base.minor < compare.minor)
            return 1;

        return 2;
    }

}