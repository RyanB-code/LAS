#include "ModuleManager.h"

#include <iostream>

ModuleManager::ModuleManager(std::shared_ptr<Logger> setLogger, ModuleSettingsPtr setSettings) :
    logger {setLogger},
    settings {setSettings}
{

}
ModuleManager::~ModuleManager(){

}
bool ModuleManager::addModule(ModulePtr module){
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
std::pair<int, StringVector> ModuleManager::loadModules(std::string directory){
    std::pair<int, StringVector> returnVariable {0 , StringVector{}};   // Initialize return variable
    directory = TextManipulations::ensureSlash(directory);              // Ensure slash at the end
    const std::filesystem::path qualifiedDirectory{directory};          // Path with slashes
    std::string moduleNameSuffix {"LASModule_"};                        // Every module name must have this key present to be added


    // Throw exception if the directory doesn't exist
    if(!std::filesystem::exists(qualifiedDirectory)){
        throw std::filesystem::filesystem_error("Directory for modules does not exist", qualifiedDirectory, std::error_code());
    }

	for(auto const& file : std::filesystem::directory_iterator(qualifiedDirectory)){
        bool failedToLoad{false};

        // Ensure file contains substring to know that it's an LASModule
        std::string fileName {file.path()};
        if(fileName.find(moduleNameSuffix) != fileName.npos){
            ModulePtr moduleBuffer {LASCore::bindFiletoModule(fileName, logger)}; // Creates Module buffer

            if(moduleBuffer){
                // Load module information
                if(moduleBuffer->load(*settings.get())){
                    if(!addModule(moduleBuffer))
                        failedToLoad = true;
                }
                else
                    failedToLoad = true;
                
            }
            else
                failedToLoad = true;
        }
        else{
           failedToLoad = true;
        }

        if(failedToLoad){
            ++returnVariable.first;
            returnVariable.second.push_back(fileName);
        }
	}

    return returnVariable;
}
const StringVector ModuleManager::getModuleNames() const{
    StringVector names;
    for(auto module : modules){
        names.push_back(module.second->getTitle());
    }
    return names;
}

// MARK: LASCore Namespace 
namespace LASCore{
    ModulePtr bindFiletoModule(std::string path, LoggerPtr logger){
        typedef bool(*loadFunction)(const ModuleSettings&, ModuleInfo&);      // Function pointer for LASM_load()
        typedef void(*voidNoParams)();                                  // Function pointer LASM_run() and LASM_cleanup()

        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        // Do not continue if library could not be opened
        if(!lib){
            return nullptr;
        }

        // Bind the API funcions
        loadFunction load       {reinterpret_cast<loadFunction>(dlsym(lib, "LASM_load"))};
        voidNoParams run        {reinterpret_cast<voidNoParams>(dlsym(lib, "LASM_run"))};
        voidNoParams cleanup    {reinterpret_cast<voidNoParams>(dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding failed
        if(!load || !run || !cleanup){
            return nullptr;
        }

        return std::make_shared<Module>(logger, load, run, cleanup);       
    }
}