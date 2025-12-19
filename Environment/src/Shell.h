#pragma once

#include "Helpers.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <LAS/Commands.h>
#include <LAS/HelperFunctions.h>
#include <LAS/Window.h>

#include <string>
#include <queue>
#include <memory>
#include <unordered_map>
#include <iostream> 

namespace LAS{ 

    class Shell {
    public:
        Shell()     = default;
        ~Shell()    = default;

        void draw();   // For drawing the window
    
        // Commands
        bool addCommandGroup    (const std::string& name);
        void removeCommandGroup (const std::string& name);
        const std::unordered_map<std::string, std::shared_ptr<Command>>& getGroup (const std::string& name) const;        // Throws out of range if not found

        bool addCommand             (const std::string& groupName,  std::shared_ptr<Command> command);                         // Adds command to list of known commands
        bool addToCommandHistory    (const std::string& text);
 
        bool getAllGroupsManuals    (std::ostringstream& os)                                const;
        bool getGroupManual         (std::ostringstream& os, const std::string& groupName)  const;
        // -------------------


        // Aliases 
        bool        addAlias            (const std::string& key, const std::string& value);
        bool        removeAlias         (const std::string& key);
        std::string findAlias           (const std::string& key);
        void        removeAllAliases    ();
        // ------------------


        // Command queue 
        void addToQueue         (const std::string& entry);
        bool handleCommandQueue (bool writeToHistory=true);
        // -----------------

        // Work with Files
        std::string getRCPath               ()  const;
        std::string getCommandHistoryPath   ()  const;

        bool        setRCPath               (const std::string& path, bool createNewFile=true);
        bool        setCommandHistoryPath   (const std::string& path);

        bool        readRCFile              (std::string path="");
        // ----------------


    private:
        std::unordered_map  <std::string, std::unordered_map<std::string, std::shared_ptr<LAS::Command>>>  commands{ };    // Holds commands by group
        std::unordered_map  <std::string, std::string>                                  aliases { };

        std::queue<std::string> commandQueue    { };
        StringVector            commandHistory  { };
        TextBuffer              consoleTextBuffer;            // Holds the text buffer for the console window

        std::string rcPath              { };
        std::string commandHistoryPath  { };
    };


    namespace ShellHelper{
        bool        readRCFile              (const std::string& path, std::queue<std::string>& queue);
        bool        defaultInitializeRCFile (const std::string& path);
        bool        writeToCommandHistory   (const std::string& path, const std::string& text);
        uint16_t    linesInFile             (const std::string& path);
        bool        retrieveLines           (const std::string& path, StringVector& cache, uint16_t cacheNumberOfLines);    // Retrieves the last X number of lines
        
        std::pair<std::string, std::string> createAlias(std::string text); // Throws invalid argument if not in proper format (aliasName="alias text")
    }

}
