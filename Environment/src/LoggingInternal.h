#pragma once

#include <LAS/Logging.h>
#include <LAS/Window.h>

#include <iostream>

namespace LAS::Logging{
    class LogToFile final : public LogOutput{
    public:
        LogToFile();
        ~LogToFile();

        std::string getPath() const;
        bool setPath(std::string setPath);      // Sets the path to the file, will not create file

        void log (const Log& log) override;
    private:
        std::string path;
    };

    class LogWindow : public Windowing::Window, public LogOutput {
    public:
        explicit LogWindow();
        ~LogWindow();

        void draw() override;
        void log(const Log& log) override;

        int getWindowID() const;
        int getLogOutputID() const;

    private:
        std::vector<Log> logHistory;
    };

    class LogToConsole : public LogOutput {
    public:
        LogToConsole();
        ~LogToConsole();

        void log(const Log& log) override;
    };
}
