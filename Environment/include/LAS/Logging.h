#pragma once

#include "HelperFunctions.h"

#include <memory>
#include <cstdint>
#include <string>
#include <source_location>
#include <chrono>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <format>


namespace LAS::Logging {

    std::string printTime       (const std::chrono::system_clock::time_point& time)     noexcept;   // HH:MM:SS
    std::string printLocation   (const std::source_location& location)                  noexcept;   // <File Name> | <Function Name> | <Line> 

    class LogSettings;
    class Logger;

    using LoggerPtr         = std::shared_ptr<Logger>;
    using LogSettingsPtr    = std::shared_ptr<LogSettings>;
    using Timepoint         = std::chrono::time_point<std::chrono::system_clock>;
    using Tags              = std::vector<std::string>;
    class LogSettings {     // Made a class to inherit type usage abilities
    public:
        bool showTime               { true };
        bool showTags               { true };
        bool showMsg                { true };
        bool showLocation           { true };

        uint8_t textBoxWidth_tag    { 5 };   
        uint8_t textBoxWidth_msg    { 30 };
    };

    class Log {
    public:
        Log (std::string setMsg, std::string setTag,    std::source_location setLocation, Timepoint setTimestamp);
        Log (std::string setMsg, Tags setTags,          std::source_location setLocation, Timepoint setTimestamp);
        ~Log();

        std::string             getMsg()        const;
        std::source_location    getLocation()   const;
        Timepoint               getTimestamp () const;
        const StringVector&     getTags()       const;

        Log& addTag(std::string tag);

    private:
        std::string 			msg;
        StringVector            tags;
        std::source_location    location;
        Timepoint          	    timestamp;
    };


    class LogOutput {
    public:
        LogOutput ();
        virtual ~LogOutput ();

        virtual void log (const Log& log, const LogSettings& logSettings) const = 0;
        uint8_t getID() const;

    private:
        uint8_t ID; // Should not even be protected to not allow derived classes access to edit
    };

    class Logger {
    public:
        Logger(LogSettingsPtr setLogSettings);
        ~Logger();

        void log(std::string setMsg, const Tags& setTag, std::source_location setLocation = std::source_location::current()) const;

        bool addOutput (std::shared_ptr<LogOutput> output);
        bool removeOutput (uint8_t ID);

        LogSettingsPtr getLogSettings ();

    private:
        LogSettingsPtr logSettings;
        std::vector<std::shared_ptr<LogOutput>> outputs;

    };
}


