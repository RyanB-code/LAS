#include "ModuleManager.h"

ModuleManager::ModuleManager(std::shared_ptr<Logger> setLogger){

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
    directory = TextManipulations::ensureSlash(directory);      // Ensure slash at the end
    const std::filesystem::path qualifiedDirectory{directory};  // Path with slashes
    std::string moduleNameSuffix {"LASModule_"};                // Every module name must have this key present to be added


    // Throw exception if the directory doesn't exist
    if(!std::filesystem::exists(qualifiedDirectory)){
        throw std::filesystem::filesystem_error("Directory for modules does not exist", qualifiedDirectory, std::error_code());
    }

    int moduleReadErrors { 0 }; // Count Modules that could not be created successfully 
	for(auto const& file : std::filesystem::directory_iterator(qualifiedDirectory)){
        // Check if file contains substring
        std::string fileName {file.path()};
        if(fileName.find(moduleNameSuffix) != fileName.npos){
            // Attempt to make Module and bind functions
            // PICK UP HERE
        }
        else{
            // Add to list of unable to load, both name and number
            ++returnVariable.first;
            returnVariable.second.push_back(fileName);
        }
	}

    return returnVariable;
}