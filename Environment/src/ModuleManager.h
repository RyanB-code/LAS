#pragma once

#include "Module.h"
#include "Helpers.h"

#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>
#include <LAS/SDK.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module

#include <string>
#include <memory>
#include <dlfcn.h>

namespace LAS{

    class ModuleManager final {
    public:
        explicit ModuleManager();
        ~ModuleManager();

        bool addModule      (ModulePtr& module);
        void removeModule   (const std::string& title);
        bool containsModule (const std::string& title) const { return modules.contains(title); }
        
        // Throws out_of_range if not found
        Module& getModule   (const std::string& title) { return *modules.at(title); }
        
        const auto& getModuleList() const { return modules; }
        const auto& getUpdateList() const { return updateFunctions; }
        auto& getDrawList() { return drawFunctions; }

        void loadAllModules ();
        bool loadModule     (const std::filesystem::path& file);
        void clearModules();

        void setupAllModules    (ImGuiContext& context);  
        bool setupModule        (ImGuiContext& context, Module& module);

        std::string getModuleLoadDirectory  () const { return loadDirectory.string(); }
        std::string getModuleFilesDirectory () const { return filesDirectory.string(); }

        bool setModuleLoadDirectory  (const std::string& directory);
        bool setModuleFilesDirectory (const std::string& directory);

    private:
        std::map<std::string, ModulePtr> modules { };
        std::map<std::string, TaggedUpdateFunction> updateFunctions   { };
        std::map<std::string, TaggedDrawFunction>   drawFunctions     { };

        std::filesystem::path loadDirectory;
        std::filesystem::path filesDirectory;

        static constexpr std::string moduleNameSuffix    {".lasm"};    // Every module must end with this to attempt to be added
    };

    using ModuleManagerPtr  = std::shared_ptr<ModuleManager>;
    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
        int         compareVersions         (const Version& base, const Version& compare);      // 0 = same major and minor, 1 = compare minor is older, 2 = compare minor is newer,  3 = fatal mismatch
        
        int         verifyModuleInformation (const ModulePtr& module);                          
        // 0 = success, 1 = bad ptr, 2 = title empty, 3 = group empty, 
        // 4 = bad group name, 5 = version warning, module minor is older than SDK, 6 =  5 = version warning, module minor is newer than SDK, 7 = fatal version mismatch               

    }
}
