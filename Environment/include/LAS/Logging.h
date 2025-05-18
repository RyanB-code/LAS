#pragma once

#include "HelperFunctions.h"

#include <format>
#include <map>
#include <source_location>


namespace LAS::Logging {
    // Forward Declarations
    class LogOutput;
    class Logger;

    // Aliases
    using ConstLoggerPtr    = std::shared_ptr<const Logger>;
    using LogOutputPtr      = std::shared_ptr<LogOutput>;

    // Free Helping Functions
    std::string printTime       (const std::chrono::system_clock::time_point& time)     noexcept;   // HH:MM:SS
    std::string printLocation   (const std::source_location& location)                  noexcept;   // <File Name> | <Function Name> | <Line> 

    struct LogSettings {     
        bool showTime               { true };
        bool showTags               { true };
        bool showMsg                { true };
        bool showLocation           { true };

        uint8_t textBoxWidth_tag    { 5 };   
        uint16_t textBoxWidth_msg    { 30 };
    };
    
    struct Log {
        const std::string           msg;
        const std::string           tag;
        const std::source_location  location;

        const std::chrono::time_point<std::chrono::system_clock> timestamp;
    };
    

    class LogOutput {
    public:
        LogOutput(const LogSettings& setSettings=LogSettings{ }); // No more than 50 outputs allowed. Throws out of range if exceeded
        virtual ~LogOutput();

        void        setSettings (const LogSettings& setSettings);
        LogSettings getSettings () const;
        int         getID       () const;

        virtual void log(const Log& log) const = 0;

    protected:
        LogSettings settings;
    private:
        int ID;
    };

    class Logger {
    public:
        ~Logger();

        static Logger& getInstance();

        void log            (const std::string& msg, const std::string& tag, const std::source_location& location=std::source_location::current()) const;
        bool addOutput      (LogOutputPtr output);
        bool contains       (int ID) const;
        bool removeOutput   (int outputID);

        void        setGlobalSettings   (const LogSettings& setSettings);           // Updates all LogOutputs to have the same settings
        bool        setOutputSettings   (int ID, const LogSettings& setSettings);

        LogSettings getGlobalSettings   () const;
        LogSettings getOutputSettings   (int ID) const; // Throws out_of_range if not there

    private:
        Logger();
        Logger(const Logger& other) = delete;
        Logger& operator=(const Logger& other) = delete;

        LogSettings settings { };
        std::map<int, LogOutputPtr> outputs { };
    };
}

namespace LAS {
    void log            (const std::string& msg, const std::string& tag, const std::source_location& location=std::source_location::current());
    void log_info       (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_warn       (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_error      (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_critical   (const std::string& msg, const std::source_location& location=std::source_location::current());

    namespace Logging {
        bool addOutput      (LogOutputPtr output);
        bool removeOutput   (int outputID);

        void        setGlobalSettings   (const LogSettings& setSettings);
        bool        setOutputSettings   (int ID, const LogSettings& setSettings);

        LogSettings getGlobalSettings   ();
        LogSettings getOutputSettings   (int ID); // Throws out_of_range if not there
    }
}
