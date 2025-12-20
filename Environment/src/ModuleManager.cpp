#include "ModuleManager.h"

using namespace LAS;


// MARK: MODULE MANAGER
ModuleManager::ModuleManager() {

}
ModuleManager::~ModuleManager(){

}
bool ModuleManager::addModule(ModulePtr& module){
    const ModuleInfo& info {module->getModuleInfo()};

    if(modules.contains(info.title))
        return false;

    if(!modules.try_emplace(info.title, std::move(module)).second)
        return false;

    TaggedUpdateFunction    update   { info.shortTag, info.updateFunction };
    TaggedDrawFunction      draw     { info.shortTag, info.drawFunction };

    if(!updateFunctions.try_emplace(info.title, update).second){
        modules.erase(info.title);
        return false;
    }

    if(!drawFunctions.try_emplace(info.title, draw).second){
        modules.erase(info.title);
        updateFunctions.erase(info.title);
        return false;
    }

    return true;
}
void ModuleManager::removeModule(const std::string& title){
    modules.erase(title);           // Module's cleanup() called on object destruction
    updateFunctions.erase(title);
    drawFunctions.erase(title);
}

void ModuleManager::loadAllModules(){
    if(!std::filesystem::exists(loadDirectory))
        throw std::filesystem::filesystem_error("Directory to load Modules does not exist", loadDirectory, std::error_code());

	for(auto const& file : std::filesystem::directory_iterator(loadDirectory)){
        if(!loadModule(file))
            log_warn(std::format("Failed to load Module from file [{}]", file.path().string() ));
	}
}
bool ModuleManager::loadModule(const std::filesystem::path& file){
    const std::string& fileName { file.string() };

    if(!fileName.ends_with(moduleNameSuffix)){
        log_warn("The file [" + fileName + "] was ignored for module loading. Improper name format.");
        return false;
    }
    
    ModulePtr moduleBuffer { };
    try{
        moduleBuffer = LAS::Modules::bindFiletoModule(fileName);
    }
    catch(std::exception& e){
        log_warn(std::format("{} from file [{}]", e.what(), fileName));
        return false;
    }

    const ModuleInfo& info { moduleBuffer->getModuleInfo() };

    // This writes library information to the buffer
    if(!moduleBuffer->loadInfo()){
        log_error("Module::loadInfo() failed from Module [" + info.title + "]. Loading procedure terminated");
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
            fatalMsg << std::format(
                    "Fatal version mismatch. Environment SDK is {}. Module [{}] made with SDK {}",
                    LAS::SDK::getVersion(),
                    info.title,
                    versionToStr(info.sdkVersion)
                );
            log_error( fatalMsg.str());
            return false;
            break;
        default:
            log_error("Failure to verify information for file [" + fileName + "]");
            return false;
            break;
    }

    log_info("Checks passed for Module [" + info.title + "] version " + versionToStr(info.moduleVersion));
 
    if(!addModule( moduleBuffer )){
        log_error("Could not add module [" + info.title + "]");
        return false;
    } 

    return true;
}
void ModuleManager::clearModules() {
    modules.clear();
    updateFunctions.clear();
    drawFunctions.clear();
}
void ModuleManager::setupAllModules(ImGuiContext& context){
    for(const auto& [title, module] : modules){
        if(!module)
            continue;

        if(!setupModule(context, *module))
            log_warn(std::format("Failed to setup Module [{}]'", module->getModuleInfo().title));
    }

    return;
}
bool ModuleManager::setupModule(ImGuiContext& context, Module& module){
    const ModuleInfo& info { module.getModuleInfo() };

    // Setup module files 
    const std::string formattedTitle  { LAS::TextManip::ensureAlNumNoSpaces(info.title) };
    std::string rcPathTitle     { formattedTitle };

    // Makes rcPath all lower case
    for(auto& c : rcPathTitle){
        if(std::isalpha(c))
            c = std::tolower(c);
    }

    std::filesystem::path moduleDirectory { filesDirectory /  LAS::TextManip::ensureSlash(formattedTitle) };
    std::filesystem::path rcFilePath      { moduleDirectory / std::string { '.' + rcPathTitle + "-rc"} };


    if(!LAS::ensureDirectory(filesDirectory)){
        log_error(
                std::format(
                    "Could not find or create directory [{}] for Module [{}]",
                    filesDirectory.string(),
                    info.title
                )
            );
        return false;
    }
    if(!LAS::ensureFile(rcFilePath)){
        log_error(
                std::format(
                    "Could not find or create file [{}] for Module [{}]",
                    rcFilePath.string(),
                    info.title
                )
            );
        return false;
    }


    EnvironmentInfo envInfo {moduleDirectory, rcFilePath, context};
    
    Logging::setModuleTag(info.shortTag);
    bool initFailed { module.init(envInfo) };
    Logging::setModuleTag("LAS");

    if(!initFailed){
        log_error("Failed loading environment info from Module [" + info.title + "]");
        return false;
    }
 
    log_info("Setup complete for Module [" + info.title + "]");
    return true;
}
bool ModuleManager::setModuleLoadDirectory(const std::string& directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    loadDirectory = std::filesystem::path { qualifiedDirectory };
    return true;
}
bool ModuleManager::setModuleFilesDirectory(const std::string& directory){
    std::string qualifiedDirectory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(qualifiedDirectory))
        return false;
    
    filesDirectory = std::filesystem::path { qualifiedDirectory };
    return true;
}





namespace LAS::Modules{

    ModulePtr bindFiletoModule(const std::string& path){
        using namespace ModuleFunctions;

        void* lib {dlopen(path.c_str(), RTLD_LAZY)};    // Map the shared object file

        if(!lib){
            log_error("Error loading module. dlopen() failed. What: " + std::string{dlerror()} );
            throw std::runtime_error{"Failed to create handle from shared object file"};     // Do not continue if library could not be opened
        }

        // Bind the API funcions
        LoadModuleInfo  loadModuleInfo  {reinterpret_cast<LoadModuleInfo>   (dlsym(lib, "LASM_loadModuleInfo"))};
        InitModule      initModule      {reinterpret_cast<InitModule>       (dlsym(lib, "LASM_init"))};
        VoidNoParams    cleanup         {reinterpret_cast<VoidNoParams>     (dlsym(lib, "LASM_cleanup"))};

        // Do not continue if binding functions failed
        if(!loadModuleInfo || !initModule ||!cleanup)
            throw std::runtime_error{"Failed to find necessary function signatures"};     // Do not continue if library could not be opened


        return std::make_unique<Module>(loadModuleInfo, initModule, cleanup);
    }
    int verifyModuleInformation(const ModulePtr& module) {
        if(!module)
            return 1;

        const ModuleInfo& info { module->getModuleInfo() };

        if(info.title.empty())
            return 2;

        std::string groupName {info.shortTag};
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
