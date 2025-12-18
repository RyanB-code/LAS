#pragma once

#include "Display.h"
#include "ModuleManager.h"
#include "Loggers.h"
#include "Shell.h"
#include "CommandsInternal.h"
#include "Config.h"

#include <LAS/Commands.h>
#include <LAS/Logging.h>
#include <LAS/HelperFunctions.h>

#include <string>
#include <chrono>
#include <thread>

namespace LAS{

    struct FilePaths{
        std::string parentDir;
        std::string dotLASDir;

        std::string logDir;
        std::string moduleLibDir;
        std::string moduleFilesDir;

        std::string rcPath;
        std::string commandHistoryPath;

        std::string imGuiIniPath;
    };
    
    namespace FrameworkSetup{
        std::string     createLogFile       (const std::string& parentDir);
        std::string     getExeParentDir     ();

        bool            setupFileLogger     (const std::string& logDir);
        bool            setupFilesystem     (FilePaths& filePaths);
    }

    class Framework final{
    public:
        Framework();
        ~Framework();

        bool setup  ();
        void run    ();

    private:
        static constexpr char       COMMAND_GROUP_NAME[]    {"las"};    // Group for all built in commands
        static constexpr int16_t    NUM_CACHED_COMMANDS     { 50 };     // How many previous commands will be added to command history upon startup
        static constexpr uint8_t    MAX_MODULES             { 32 };

        // Only change the divisor, since that is the target per second
        static constexpr std::chrono::milliseconds UPDATE_INTERVAL  { 1000 / 100 }; // Update 100 times per second 
        static constexpr std::chrono::milliseconds FRAME_INTERVAL   { 1000 / 60 };  // Refresh frame 60 times a second 

        // Wakeup every X ms
        static constexpr std::chrono::milliseconds SLEEP_INTERVAL   { 3 };

        bool setupComplete { false };

        std::shared_ptr<ModuleManager>      moduleManager;
        std::shared_ptr<DisplayManager>     displayManager;
        std::shared_ptr<Shell>              shell;
        std::shared_ptr<Display::LogWindow> logWindow { }; // Needed to store logs

        std::pair<bool, int> setupLoggers();

        bool setupShell             (const std::string& rcPath, const std::string& commandHistoryPath);
        bool setupModuleManager     (const std::string& moduleLoadDir, const std::string& moduleFilesDir);
        void setupInternalWindows   ();
        bool setupDisplay           (const std::string& iniFilePath);
        void setupCommands          (); // Instantiate commands here                            

        bool loadAllModules         ();
        void loadAllModuleFunctions ();
        void loadAllModuleCommands  ();                                 
        void setupAllModules        ();
                                        
        void removeModule           (const std::string& title);

        void readAllModuleRCFiles   ();
        bool readModuleRCFile       (const std::string& name);

    };
}
