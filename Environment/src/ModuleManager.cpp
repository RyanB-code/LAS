#include "ModuleManager.h"

using namespace LAS;

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
    static constexpr  std::string moduleNamePrefix    {"LASModule_"};         // Every module name must have this key present to be added

    if(fileName.find(moduleNamePrefix) == fileName.npos)
        return false;
    
    ModulePtr moduleBuffer{LAS::Modules::bindFiletoModule(fileName, logger, context)};
        
    if(!moduleBuffer)
        return false;

    if(!moduleBuffer->loadModuleInfo()){
        logger->log("Failed loading module info from Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }
   
    std::string formattedTitle { LAS::TextManip::ensureAlNumNoSpaces(moduleBuffer->getTitle())};
    parentDirectory    = LAS::TextManip::ensureSlash(parentDirectory);

    std::string moduleFilesDirectory    {LAS::TextManip::ensureSlash(parentDirectory) + LAS::TextManip::ensureSlash(formattedTitle)};
    std::string moduleRCFilePath        { moduleFilesDirectory + '.' + formattedTitle + "-rc"};

    if(!LAS::ensureDirectory(moduleFilesDirectory)){
        logger->log("Could not find or create directory [" + moduleFilesDirectory + "] for Module + [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }
    if(!LAS::ensureFile(moduleRCFilePath)){
        logger->log("Could not find or create file [" + moduleRCFilePath + "] for Module + [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }

    // Pass environment info only if all files could be created
    EnvironmentInfo envInfo {moduleFilesDirectory, moduleRCFilePath, context, logger};
    if(!moduleBuffer->loadEnvInfo(envInfo)){
        logger->log("Failed loading environment info from Module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
        return false;
    }

    if(!addModule(moduleBuffer)){
        logger->log("could not add module [" + moduleBuffer->getTitle() + "]", Tags{"ERROR", "Module Manager"});
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
        if(itr->second->getWindow()->getMenuOption() != LAS::MenuOption::UTILITY)
           modulesToErase.push_back(itr);
    }

    for(auto i : modulesToErase)
        modules.erase(i);
}

// MARK: LASCore Namespace 
namespace LAS::Modules{

    ModulePtr bindFiletoModule(const std::string& path, const LoggerPtr& logger, const ImGuiContext& context){
        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        if(!lib)
            return nullptr;     // Do not continue if library could not be opened

        // Bind the API funcions
        LoadModuleInfo      loadModuleInfo    {reinterpret_cast<LoadModuleInfo>     (dlsym(lib, "LASM_loadModuleInfo"))};
        LoadEnvironmentInfo loadEnvInfo       {reinterpret_cast<LoadEnvironmentInfo>(dlsym(lib, "LASM_loadEnvironmentInfo"))};
        VoidNoParams        cleanup           {reinterpret_cast<VoidNoParams>       (dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding failed
        if(!loadModuleInfo || !loadEnvInfo ||!cleanup)
            return nullptr;

        return std::make_shared<Module>(logger, loadModuleInfo, loadEnvInfo, cleanup);       
    }
    bool verifyModuleInformation(const EnvironmentInfo& envInfo, const ModulePtr& module) {
        if(!module)
            return false;

        if(module->getTitle().empty())
            return false;

        std::string groupName {module->getGroupName()};
        if(groupName.empty())
            return false;
        
        for(const auto& c : groupName){
            if(!std::isalnum(c))
                return false;
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

}