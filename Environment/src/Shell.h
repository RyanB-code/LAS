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

    private:
        std::queue<std::string>& queue;

        std::ostringstream  textHistory;
        StringVector        commandHistory;
    };

    class Shell{
    public:
        explicit Shell(std::shared_ptr<ConsoleWindow> = nullptr);
        ~Shell();

        bool addCommand         (CommandPtr command);              // Adds command to list of known commands

        bool addOutput          (const ShellOutputPtr& output);
        bool removeOutput       (const uint8_t& getID);

        void addToQueue         (const std::string& entry);
        bool handleCommandQueue ();

        bool readRCFile         (const std::string& path);

        const std::unordered_map<std::string, CommandPtr>& viewCommandInfo();

        std::shared_ptr<ConsoleWindow> getWindow();

    private:
        std::unordered_map<std::string, CommandPtr> commands;
        std::queue      <std::string>               commandQueue;
        std::vector     <ShellOutputPtr>            outputs;
        std::shared_ptr <ConsoleWindow>             window;
    };

}
