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

using ShellPtr = std::shared_ptr<LAS::Shell>;
using ShellOutputPtr = std::shared_ptr<LAS::ShellOutput>;

namespace LAS{
    class ShellOutput{
    public:
        ShellOutput();
        virtual ~ShellOutput();

        virtual void output(const std::ostringstream&);

        uint8_t getID() const;

    private:
        uint8_t ID;
    };

    class ConsoleWindow : public LAS::Window, public ShellOutput {
    public:
        ConsoleWindow(std::queue<std::string>& setQueue);
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
        Shell(std::shared_ptr<ConsoleWindow> = nullptr);
        ~Shell();

        bool readRCFile         (const std::string& path);
        bool addCommand         (CommandPtr command);              // Adds to commands unoredered_map
        bool handleCommandQueue ();

        bool addOutput(ShellOutputPtr output);
        bool removeOutput(uint8_t getID);

        void addToQueue         (const std::string& entry);

        std::shared_ptr<ConsoleWindow> getWindow();

    private:
        std::unordered_map<std::string, CommandPtr> commands;
        std::queue<std::string>                     commandQueue;

        std::vector<ShellOutputPtr>                 outputs;

        std::shared_ptr<ConsoleWindow>              window;
    };

}
