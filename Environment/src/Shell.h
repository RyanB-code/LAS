#pragma once

#include "Display.h"
#include "ModuleManager.h"

#include <imgui/imgui.h>
#include <LAS/Commands.h>
#include <LAS/Helper.h>
#include <LAS/Window.h>

#include <string>
#include <map>
#include <queue>
#include <memory>
#include <vector>

// FOR TESTING
#include <iostream>

// Forward Declarations
namespace LAS{
    class ShellOutput;
    class Shell;
}

using ShellPtr          = std::shared_ptr<LAS::Shell>;
using ShellOutputPtr    = std::shared_ptr<LAS::ShellOutput>;

namespace LAS{
    class ShellOutput{
    public:
        ShellOutput();
        virtual ~ShellOutput();

        virtual void output(const std::ostringstream&)  = 0;
        virtual void output(const std::string&)         = 0;

        uint8_t getID() const;

    private:
        uint8_t ID;
    };

    class ConsoleWindow : public Window, public ShellOutput {
    public:
        explicit ConsoleWindow(std::queue<std::string>& setQueue);
        ~ConsoleWindow();

        void draw() override;
        void output(const std::ostringstream& os)   override;
        void output(const std::string& msg)         override;

        bool addToCommandHistory (const std::string& text);

    private:
        std::queue<std::string>& queue;

        std::ostringstream  textHistory;
        StringVector        commandHistory;
    };

    class Shell{
    public:
        explicit Shell(std::shared_ptr<ConsoleWindow> = nullptr);
        ~Shell();
    
        bool addCommandGroup    (const std::string& name);
        bool removeCommandGroup (const std::string& name);
        const std::unordered_map<std::string, CommandPtr>& getGroup (const std::string& name) const;        // Throws out of range if not found

        bool addCommand         (const std::string& groupName, CommandPtr command);                         // Adds command to list of known commands

        bool addAlias           (const std::string& key, const std::string& value);
        bool removeAlias        (const std::string& key);
        std::string findAlias   (const std::string& key);

        bool addOutput          (const ShellOutputPtr& output);
        bool removeOutput       (const uint8_t& getID);
        void reportToAllOutputs (const std::string& msg)        const;       
        void reportToAllOutputs (const std::ostringstream& msg) const;

        void addToQueue         (const std::string& entry);
        bool handleCommandQueue (bool writeToHistory=true);

        bool readRCFile         (const std::string& path);          // Creates default RC file if it does not exist

        bool getAllGroupsManuals    (std::ostringstream& os)                                const;
        bool getGroupManual         (std::ostringstream& os, const std::string& groupName)  const;

        std::shared_ptr<ConsoleWindow>  getWindow()                 const;
        std::string                     getRCPath()                 const;
        std::string                     getCommandHistoryPath()     const;

        bool                setRCPath               (const std::string& path);
        bool                setCommandHistoryPath   (const std::string& path);
    private:
        std::unordered_map  <std::string, std::unordered_map<std::string, CommandPtr>>  commands;
        std::unordered_map  <std::string, std::string>                                  aliases;

        std::queue          <std::string>               commandQueue;
        std::vector         <ShellOutputPtr>            outputs;
        std::shared_ptr     <ConsoleWindow>             window;


        std::string rcPath;
        std::string commandHistoryPath;
    };

    namespace ShellHelper{
        bool        defaultInitializeRCFile (const std::string& path);
        bool        writeToCommandHistory   (const std::string& path, const std::string& text);
        uint16_t    linesInFile             (const std::string& path);
        bool        retrieveLines           (const std::string& path, StringVector& cache, uint16_t cacheNumberOfLines);    // Retrieves the last X number of lines
        
        std::pair<std::string, std::string> createAlias(std::string text); // Throws invalid argument if not in proper format (aliasName="alias text")
    }

}
