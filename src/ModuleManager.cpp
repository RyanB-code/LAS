#include "ModuleManager.h"

ModuleManager::ModuleManager(std::string setModduleDirectory, std::shared_ptr<Logger> setLogger){

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
std::pair<int, StringVector> ModuleManager::loadModules(){

}