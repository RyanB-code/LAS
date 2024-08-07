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
StringVector ModuleManager::loadModules(ImGuiContext& context, std::string directory){
    if(moduleDirectory.empty())
        directory = moduleDirectory;

    std::string     moduleNameSuffix    {"LASModule_"};         // Every module name must have this key present to be added
    StringVector    failedToLoad        {};                     // Initialize empty return variable

    directory = LAS::TextManip::ensureSlash(directory);              // Ensure slash at the end
    const std::filesystem::path qualifiedDirectory{directory};       // Path with slashes


    // Throw exception if the directory doesn't exist
    if(!std::filesystem::exists(qualifiedDirectory)){
        throw std::filesystem::filesystem_error("Directory for modules does not exist", qualifiedDirectory, std::error_code());
    }

    // Create what to pass
    EnvironmentInfo pass {directory, context, logger};

    std::vector<ModulePtr> modules;
	for(auto const& file : std::filesystem::directory_iterator(qualifiedDirectory)){
        bool failed {false};
        // Ensure file contains substring to know that it's an LASModule
        std::string fileName {file.path()};
        if(fileName.find(moduleNameSuffix) != fileName.npos){
            ModulePtr moduleBuffer {LAS::Modules::bindFiletoModule(fileName, logger, context)}; // Creates Module buffer
        
            if(moduleBuffer){
                if(moduleBuffer->load(pass)){       // Pass info across DLL boundary
                    if(!addModule(moduleBuffer))    // Add to known modules
                        failed = true;
                }
                else
                    failed = true;
            }            else
                failed = true;
        }
        else{
           failed = true;
        }

        if(failed){
            failedToLoad.push_back(fileName);
        }
	}

    return failedToLoad;
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
        list.push_back(module.second->getInfo().window);
    }

    return list;
}
std::string ModuleManager::getModuleDirectory() const{
    return moduleDirectory;
}
bool ModuleManager::setModuleDirectory(const std::string& directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);              // Ensure slash at the end

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    moduleDirectory = qualifiedDirectory;

    return std::filesystem::exists(qualifiedDirectory);
}

// MARK: LASCore Namespace 
namespace LAS::Modules{
    ModulePtr bindFiletoModule(std::string path, const LoggerPtr& logger, ImGuiContext& context){
        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        if(!lib)
            return nullptr;     // Do not continue if library could not be opened

        // Bind the API funcions
        loadFunction load       {reinterpret_cast<loadFunction>(dlsym(lib, "LASM_load"))};
        voidNoParams cleanup    {reinterpret_cast<voidNoParams>(dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding failed
        if(!load || !cleanup)
            return nullptr;

        return std::make_shared<Module>(logger, load, cleanup);       
    }
}