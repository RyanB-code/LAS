#include <LAS/Logging.h>
#include <LAS/Window.h>

class LogToFile final : public LogOutput{
public:
    LogToFile();
    ~LogToFile();

    std::string getPath() const;
    bool setPath(std::string setPath);      // Sets the path to the file, will not create file

    bool log (const Log& log, const LogSettings& logSettings) const override;
private:
    std::string path;
};

class LogWindow : public LAS::Window {
public:
    LogWindow(LogSettingsPtr);
    ~LogWindow();

    void draw() override;

    void addLog(const Log&);

private:
    std::vector<Log> logHistory;
    LogSettingsPtr logSettings;
};

class LogToWindow final : public LogOutput {
public:
    LogToWindow(std::shared_ptr<LogWindow>);
    ~LogToWindow();

    bool log(const Log&, const LogSettings&) const override;
    std::shared_ptr<LogWindow> getWindow();

private:
    std::shared_ptr<LogWindow> window;
};