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

        virtual void output(const std::ostringstream&) = 0;

        uint8_t getID() const;

    private:
        uint8_t ID;
    };

    class ConsoleWindow : public Window, public ShellOutput {
    public:
        explicit ConsoleWindow(std::queue<std::string>& setQueue);
        ~ConsoleWindow();

        void draw() override;
        void output(const std::ostringstream& os) override;

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

        bool addCommand         (CommandPtr command);               // Adds command to list of known commands

        bool addOutput          (const ShellOutputPtr& output);
        bool removeOutput       (const uint8_t& getID);

        void addToQueue         (const std::string& entry);
        bool handleCommandQueue (bool writeToHistory=true);

        bool readRCFile         (const std::string& path);          // Creates if it does not exist

        const std::unordered_map<std::string, CommandPtr>& viewCommandInfo();

        std::shared_ptr<ConsoleWindow>  getWindow()                 const;
        std::string                     getRCPath()                 const;
        std::string                     getCommandHistoryPath()     const;

        bool                setRCPath               (const std::string& path);
        bool                setCommandHistoryPath   (const std::string& path);

    private:
        std::unordered_map  <std::string, CommandPtr>   commands;
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
    }

}
