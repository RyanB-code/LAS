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

        void log (const Log& log, const LogSettings& logSettings) const override;
    private:
        std::string path;
    };

    class LogWindow : public Windowing::Window {
    public:
        explicit LogWindow(const LogSettingsPtr&);
        ~LogWindow();

        void draw() override;

        void addLog(const Log&);

    private:
        std::vector<Log> logHistory;
        LogSettingsPtr logSettings;
    };

    class LogToWindow final : public LogOutput {
    public:
        explicit LogToWindow(std::shared_ptr<LogWindow>);
        ~LogToWindow();

        void log(const Log&, const LogSettings&) const override;
        std::shared_ptr<LogWindow> getWindow();

    private:
        const std::shared_ptr<LogWindow> window;
    };
}
