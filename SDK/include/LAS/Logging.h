#pragma once

#include "HelperFunctions.h"

#include <format>
#include <map>
#include <source_location>


namespace LAS::Logging {

    struct LogSettings {     
        bool showTime               { true };
        bool showSeverityTag        { true };
        bool showModuleTag          { true };
        bool showMsg                { true };
        bool showLocation           { true };

        int textBoxWidth_tag    { 5 };   
        int textBoxWidth_msg   { 30 };
    };
    
    struct Log {
        const std::string           msg;
        const std::string           severityTag;
        const std::string           moduleTag;
        const std::source_location  location;

        const std::chrono::time_point<std::chrono::system_clock> timestamp;
    };

    class LogOutput {
    public:
        LogOutput(const LogSettings& setSettings=LogSettings{ }); // No more than 50 outputs allowed. Throws out of range if exceeded
        virtual ~LogOutput() = default;

        void        setSettings (const LogSettings& setSettings);
        LogSettings getSettings () const;
        int         getID       () const;

        bool        isEnabled   () const;
        void        enable      ();
        void        disable     ();

        virtual void log(const Log& log) = 0;

    protected:
        LogSettings settings { };
    private:
        int ID;
        bool enabled { true };
    };

    using LogOutputPtr      = std::shared_ptr<LogOutput>;


    // For Modules to use to interact with logger
    bool addOutput      (LogOutputPtr output);
    bool removeOutput   (int outputID);

    void        setGlobalSettings   (const LogSettings& setSettings);
    bool        setOutputSettings   (int ID, const LogSettings& setSettings);

    LogSettings getGlobalSettings   ();
    LogSettings getOutputSettings   (int ID); // Throws out_of_range if not there
        
    bool enableOutput(int ID);
    bool disableOutput(int ID);


    // Helper Functions
    std::string printTime       (const std::chrono::system_clock::time_point& time)     noexcept;   // HH:MM:SS
    std::string printLocation   (const std::source_location& location)                  noexcept;   // <File Name> | <Function Name> | <Line> 

    // Change the tag for the logger
    void setModuleTag   (std::string tag);
}

namespace LAS {
    void log            (const std::string& msg, const std::string& tag, const std::source_location& location=std::source_location::current());
    void log_info       (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_warn       (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_error      (const std::string& msg, const std::source_location& location=std::source_location::current());
    void log_critical   (const std::string& msg, const std::source_location& location=std::source_location::current());

    
}
