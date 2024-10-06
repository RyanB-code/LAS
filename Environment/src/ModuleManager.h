#pragma once

#include "Module.h"

#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>
#include <LAS/SDK.h>

#include <imgui/imgui_internal.h>   // Needed for ImGuiContext passing to Module

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>
#include <queue>

#include <iostream>

namespace LAS{

    class ModuleMessageSubscriber{
    public:
        ModuleMessageSubscriber();
        virtual ~ModuleMessageSubscriber();

        virtual void notifiy() = 0;

        uint8_t getID() const;
    private:
        uint8_t ID;    
    };

    class ModuleMessageSubscriberTest : public ModuleMessageSubscriber{
    public:
        ModuleMessageSubscriberTest();
        ~ModuleMessageSubscriberTest();

        virtual void notifiy() override;
    };

    using ModMsgSubPtr = std::shared_ptr<ModuleMessageSubscriber>;

    class ModuleMessageManager{
    public:
        ModuleMessageManager();
        ~ModuleMessageManager();

        void    addMsg              (std::string msg);
        bool    addSubscriber       (ModMsgSubPtr subscriber);

        void    retrieveMessages    (uint8_t ID, std::queue<std::string>& messages);
        void    viewMessages        (std::ostringstream& os);

    private:
        static constexpr uint64_t MAX_MODULES   { 20 };

        std::vector<std::pair<std::string, std::vector<uint8_t>>>   messageQueue;
        std::vector<ModMsgSubPtr>                                   subscribers;
    };


    class ModuleManager final {
    public:
        explicit ModuleManager(const LoggerPtr& setLogger);
        ~ModuleManager();

        bool addModule      (const ModulePtr& module);
        bool removeModule   (std::string title);
        bool containsModule (std::string title)             const;

        ModulePtr           getModule(std::string title)    const;
        const StringVector  getModuleNames()                const;

        WindowList          getAllWindows()                 const;

        std::string         getModuleLoadDirectory  () const;
        std::string         getModuleFilesDirectory () const;
        bool                setModuleLoadDirectory  (const std::string& directory);
        bool                setModuleFilesDirectory (const std::string& directory);

        void loadAllModules  (ImGuiContext& context, StringVector& modulesNotLoaded, std::string loadDirectory="", std::string filesDirectory=""); // Clears the StringVector first
        bool loadModule      (std::string moduleFilesDirectory, ImGuiContext& context, const std::string& fileName);

        void clearNonUtilityModules();

        void addMsg             (const std::string& msg);
        void viewMessages       (std::ostringstream& os);
        bool addSubscriber      (ModMsgSubPtr subscriber);
        void retrieveMessages   (uint8_t ID, std::queue<std::string>& messages);

    private:
        std::unordered_map<std::string, ModulePtr> modules{};
        const LoggerPtr& logger;

        std::string moduleLoadDirectory;
        std::string moduleFilesDirectory;

        static constexpr std::string moduleNameSuffix    {".lasm"};    // Every module must end with this to attempt to be added

        ModuleMessageManager messageManager;
    };

    using ModuleManagerPtr  = std::shared_ptr<ModuleManager>;
    namespace Modules{
        ModulePtr   bindFiletoModule        (const std::string& path, const LoggerPtr& logger, const ImGuiContext& context);
        bool        ensureModuleFiles       (std::string parentDirectory, std::string moduleTitle);
        int         compareVersions         (const Version& base, const Version& compare);      // 0 = success, 1 = warning mismatch, 2 = fatal mismatch
        int         verifyModuleInformation (const ModulePtr& module);                          // 0 = success, 1 = bad ptr, 2 = title empty, 3 = group empty, 4 = bad group name, 5 = version warning, 6 = fatal version mismatch               

    }
}