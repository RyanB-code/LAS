#pragma once

#include "TextManipulations.h"

#include <memory>
#include <cstdint>
#include <string>
#include <source_location>
#include <chrono>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <format>

class LogSettings;
class Logger;

using LoggerPtr         = std::shared_ptr<Logger>;
using LogSettingsPtr    = std::shared_ptr<LogSettings>;
using Timepoint         = std::chrono::time_point<std::chrono::system_clock>;
using StringVector      = std::vector<std::string>;
using Tags              = std::vector<std::string>;



class LogSettings {     // Made a class to inherit type usage abilities
public:
    bool showTime               { true };
    bool showTags               { true };
    bool showMsg                { true };
    bool showLocation           { true };

    int8_t timeOffset           { -7 };
    uint8_t textBoxWidth_tag    { 5 };   
    uint8_t textBoxWidth_msg    { 30 };

    std::string logFilePath {};
};

class Log {
public:
    Log (std::string setMsg, std::string setTag, std::source_location setLocation, Timepoint setTimestamp);
    ~Log();

    std::string             getMsg()        const;
    std::source_location    getLocation()   const;
    Timepoint               getTimestamp () const;
    const StringVector&     getTags()       const;

    Log& addTag(std::string tag);

private:
    std::string 			msg;
    std::source_location    location;
    Timepoint          	    timestamp;
    StringVector            tags;
};


class LogOutput {
public:
    LogOutput ();
    virtual ~LogOutput ();

    virtual bool log (const Log& log, const LogSettings& logSettings) const = 0;
    uint8_t getID() const;

private:
    uint8_t ID; // Should not even be protected to now allow derived classes access to edit
};

class Logger {
public:
    Logger(LogSettingsPtr setLogSettings);
    ~Logger();

    void log(std::string setMsg, const StringVector& setTag, std::source_location setLocation = std::source_location::current()) const;

    bool addOutput (std::shared_ptr<LogOutput> output);
    bool removeOutput (uint8_t ID);

    const LogSettings& getLogSettings () const;

private:
    LogSettingsPtr logSettings;
    std::vector<std::shared_ptr<LogOutput>> outputs;

};

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