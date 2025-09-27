#include "ModuleManager.h"

using namespace LAS;


// MARK: MODULE MANAGER
ModuleManager::ModuleManager() {

}
ModuleManager::~ModuleManager(){

}
bool ModuleManager::addModule(const ModulePtr& module){
    const ModuleInfo& info {module->getModuleInfo()};

    // Check if modules contains a module of the same title before adding
    if(modules.contains(info.title)){
        return false;
    }
    else{
        modules.try_emplace(info.title, module);
        return modules.contains(info.title);        // verify module has been added successfully by checking and returning
    }
}
bool ModuleManager::removeModule(std::string title){
    if(modules.contains(title)){
        modules.erase(title);               // cleanup() is automatically called so do not need to specifically call it
        return !modules.contains(title);    // Return the inverse of contain() -> ie if modules still contains a member of the key (contains returns true), the return variable will be false since the erase did not work correctly
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
void ModuleManager::loadAllModules(StringVector& modulesNotLoaded){
    modulesNotLoaded.erase(modulesNotLoaded.begin(), modulesNotLoaded.end());

    const   std::filesystem::path   qualifiedDirectory  {LAS::TextManip::ensureSlash(moduleLoadDirectory)};   // Path with slashes

    if(!std::filesystem::exists(qualifiedDirectory)){
        throw std::filesystem::filesystem_error("Directory for modules does not exist", qualifiedDirectory, std::error_code());
    }

    // Iterate over directory and attempt to load each file
	for(auto const& file : std::filesystem::directory_iterator(qualifiedDirectory)){
        if(!loadModule(moduleFilesDirectory, file.path()))
            modulesNotLoaded.push_back(file.path());
	}

    return;
}
bool ModuleManager::loadModule  (std::string parentDirectory, const std::string& fileName){

    // Reject if not named correctly
    if(!fileName.ends_with(moduleNameSuffix)){
        log_info("The file [" + fileName + "] was ignored for module loading. Improper name format.");
        return false;
    }
    
    ModulePtr moduleBuffer;
    try{
        moduleBuffer = LAS::Modules::bindFiletoModule(fileName);
    }
    catch(std::exception& e){
        log_warn(std::string{e.what()} + " from file [" + fileName + ".");
        return false;
    }

    const ModuleInfo& info { moduleBuffer->getModuleInfo() };

    // This writes library information to the buffer
    if(!moduleBuffer->loadModuleInfo()){
        log_error("loadModuleInfo() returned false from Module [" + info.title + "]. Loading procedure terminated");
        return false;
    }

    // Verify module information
    std::ostringstream fatalMsg;
    switch(Modules::verifyModuleInformation(moduleBuffer)){
        case 0:
            // Success
            break;
         case 1:
            log_error("Module buffer does not exist. From file [" + fileName + "]");
            return false;
            break;
        case 2:
            log_error("Module title empty from file [" + fileName + "]");
            return false;
            break;
        case 3:
            log_error("Command group is empty from Module [" + info.title + "]");
            return false;
            break;
        case 4:
            log_error("Command group name rejected from Module [" + info.title + "]");
            return false;
            break;
        case 5:
        case 6:
            log_warn("SDK version mismatch. Issues may occur. Module [" + info.title + "] made with SDK version " + versionToStr(info.sdkVersion) + "."
                        "Environment SDK is version  " + LAS::SDK::getVersion());
            break;
        case 7:
            fatalMsg    << "Fatal version mismatch. Environment SDK is " << LAS::SDK::getVersion() 
                        << ". Module [" << info.title 
                        << "] SDK version is " << versionToStr(info.sdkVersion);
            log_error( fatalMsg.str());
            return false;
            break;
        default:
            log_error("Failure to verify information for file [" + fileName + "]");
            return false;
            break;
    }

    log_info("Checks passed for Module [" + info.title + "] version " + versionToStr(info.moduleVersion));
 
    if(!addModule(moduleBuffer)){
        log_error("Could not add module [" + info.title + "]");
        return false;
    } 

    return true;
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
bool ModuleManager::setupModule(ImGuiContext& context, const std::string& title, std::shared_ptr<bool> shown){
    // Pass environment info only if all files could be created
    Module& module { *getModule(title) };
    const ModuleInfo& info { module.getModuleInfo() };

    // Setup module files 
    std::string formattedTitle  { LAS::TextManip::ensureAlNumNoSpaces(info.title) };
    std::string parentDirectory { LAS::TextManip::ensureSlash(moduleFilesDirectory) };

    std::string rcPathTitle { formattedTitle };

    // Makes rcPath all lower case
    for(auto& c : rcPathTitle){
        if(std::isalpha(c))
            c = std::tolower(c);
    }

    std::string filesDirectory    {LAS::TextManip::ensureSlash(parentDirectory) + LAS::TextManip::ensureSlash(formattedTitle)};
    std::string rcFilePath        {filesDirectory + '.' + rcPathTitle + "-rc"};

    // Check directories can be made
    if(!LAS::ensureDirectory(filesDirectory)){
        log_error("Could not find or create directory [" + filesDirectory + "] for Module [" + info.title + "]");
        return false;
    }
    if(!LAS::ensureFile(rcFilePath)){
        log_error("Could not find or create file [" + rcFilePath + "] for Module [" + info.title + "]");
        return false;
    }


    EnvironmentInfo envInfo {filesDirectory, rcFilePath, context, shown};
    if(!module.loadEnvInfo(envInfo)){
        log_error("Failed loading environment info from Module [" + info.title + "]");
        return false;
    }
 
    log_info("Setup complete for Module [" + info.title + "]");
    return true;
}


void ModuleManager::clearModules() {
    modules.clear();
}
std::unordered_map<std::string, ModulePtr>::const_iterator ModuleManager::cbegin() const{
    return modules.cbegin();
}
std::unordered_map<std::string, ModulePtr>::const_iterator ModuleManager::cend() const {
    return modules.cend();
}


// MARK: LASCore Namespace 
namespace LAS::Modules{

    ModulePtr bindFiletoModule(const std::string& path){
        using namespace ModuleFunctions;

        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        if(!lib){
            log_error("Error loading module. dlopen() failed. What: " + std::string{dlerror()} );
            throw std::runtime_error{"Failed to create handle from shared object file"};     // Do not continue if library could not be opened
        }

        // Bind the API funcions
        LoadModuleInfo      loadModuleInfo    {reinterpret_cast<LoadModuleInfo>     (dlsym(lib, "LASM_loadModuleInfo"))};
        LoadEnvironmentInfo loadEnvInfo       {reinterpret_cast<LoadEnvironmentInfo>(dlsym(lib, "LASM_init"))};
        VoidNoParams        cleanup           {reinterpret_cast<VoidNoParams>       (dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding functions failed
        if(!loadModuleInfo || !loadEnvInfo ||!cleanup)
            throw std::runtime_error{"Failed to find necessary function signatures"};     // Do not continue if library could not be opened


        return std::make_shared<Module>(loadModuleInfo, loadEnvInfo, cleanup);
    }
    int verifyModuleInformation(const ModulePtr& module) {
        if(!module)
            return 1;

        const ModuleInfo& info { module->getModuleInfo() };

        if(info.title.empty())
            return 2;

        std::string groupName {info.commandGroupName};
        if(groupName.empty())
            return 3;
        
        for(const auto& c : groupName){
            if(!std::isalnum(c))
                if(c != '-')
                    return 4;
        }

        const Version sdkVersion    {LAS::SDK::getVersionMajor(), LAS::SDK::getVersionMinor(), LAS::SDK::getVersionPatch()};
        const Version moduleVersion {info.sdkVersion};
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
        case 3:
            return 7;
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
            return 3;
        
        if(base.minor < compare.minor)
            return 2;

        if(base.minor > compare.minor)
            return 1;

         if(base.minor == compare.minor)
            return 0;

        return 3;
    }

}
