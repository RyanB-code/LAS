#include <LAS/Logging.h>

namespace LAS::Logging {

    // Logger in here since not needed in API
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

        bool enableOutput(int ID);
        bool disableOutput(int ID);

    private:
        Logger();
        Logger(const Logger& other) = delete;
        Logger& operator=(const Logger& other) = delete;

        LogSettings settings { };
        std::map<int, LogOutputPtr> outputs { };
    };

    std::string printTime(const std::chrono::system_clock::time_point& time) noexcept {
        std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
        return std::format("{:%T}", zonedTime).substr(0, 8);
    }
    std::string printLocation(const std::source_location& location)	noexcept {
        std::ostringstream os;
        os << "[" << location.file_name() << " | " << location.function_name() << " | " << location.line() << "]";
        return os.str();
    }


    // LogOutput
    LogOutput::LogOutput(const LogSettings& setSettings) : settings { setSettings } {
        static int givenIDs { 0 };

        if(givenIDs < 0 || givenIDs > 50) 
            throw::std::out_of_range("Max LogOutputs reached: 50");

        ID = ++givenIDs;
    }
    LogOutput::~LogOutput(){

    }

    void LogOutput::setSettings(const LogSettings& setSettings){
        settings = setSettings;
    }
    LogSettings LogOutput::getSettings() const {
        return settings;
    }
    int LogOutput::getID() const {
        return ID;
    }
    bool LogOutput::isEnabled() const {
        return enabled;
    }
    void LogOutput::enable() {
        enabled = true;
    }
    void LogOutput::disable() {
        enabled = false;
    }

    // Logger
    Logger::Logger() {

    }
    Logger::~Logger(){

    }
    Logger& Logger::getInstance() {
        static Logger instance;
        return instance;
    }
    void Logger::log(const std::string& msg, const std::string& tag, const std::source_location& location) const {
        if(outputs.empty()){
            std::puts("There are no log outputs.");
            return;
        }

        const std::chrono::zoned_time time { std::chrono::current_zone(), std::chrono::system_clock::now() };

        Log log {msg, tag, location, time};

        for(const auto& [ID, outputPtr] : outputs){
            if(outputPtr->isEnabled())
                outputPtr->log(log);
        }

        return;
    }
    bool Logger::addOutput(LogOutputPtr newOutput) {
        if(outputs.contains(newOutput->getID()))
            return false;

        return outputs.try_emplace(newOutput->getID(), newOutput).second;
    }
    bool Logger::contains(int ID) const {
        return outputs.contains(ID);
    }
    bool Logger::removeOutput(int ID) {
        auto iterator { outputs.find(ID) };

        if(iterator != outputs.end()){
            outputs.erase(iterator);
            return true;
        }

        return false;
    }
    void Logger::setGlobalSettings(const LogSettings& setSettings){
        settings = setSettings;

        for(auto& [ID, outputPtr] : outputs)
            outputPtr->setSettings(settings);
    }
    bool Logger::setOutputSettings(int ID, const LogSettings& setSettings){
        if(!outputs.contains(ID))
            return false;

        outputs.at(ID)->setSettings(setSettings);

        return true;
    }
    LogSettings Logger::getGlobalSettings() const {
        return settings;
    }
    LogSettings Logger::getOutputSettings(int ID) const{
        if(!outputs.contains(ID))
            throw std::out_of_range("A LogOutput with that ID is not active");

        return outputs.at(ID)->getSettings();
    }
    bool Logger::enableOutput(int ID) {
        if(!outputs.contains(ID))
            return false;

        outputs.at(ID)->enable();
        return true;
    }
    bool Logger::disableOutput(int ID) {
        if(!outputs.contains(ID))
            return false;

        outputs.at(ID)->disable();
        return true;
    }
}

// LAS namespace
namespace LAS{
    void log(const std::string& msg, const std::string& tag, const std::source_location& location){
        Logging::Logger::getInstance().log(msg, tag, location);
    }
    void log_info(const std::string& msg, const std::source_location& location) {
        Logging::Logger::getInstance().log(msg, "INFO", location);
    }
    void log_warn(const std::string& msg, const std::source_location& location) {
        Logging::Logger::getInstance().log(msg, "WARNING", location);
    }
    void log_error(const std::string& msg, const std::source_location& location) {
        Logging::Logger::getInstance().log(msg, "ERROR", location);
    }
    void log_critical(const std::string& msg, const std::source_location& location) {
        Logging::Logger::getInstance().log(msg, "CRITICAL", location);
    }

    // Logging
    bool Logging::addOutput(LogOutputPtr newOutput) {
        return Logger::getInstance().addOutput(newOutput);
    }
    bool Logging::removeOutput(int ID) {
        return Logger::getInstance().removeOutput(ID);
    }
    void Logging::setGlobalSettings(const LogSettings& setSettings){
        return Logger::getInstance().setGlobalSettings(setSettings);
    }
    bool Logging::setOutputSettings(int ID, const LogSettings& setSettings){
        return Logger::getInstance().setOutputSettings(ID, setSettings);
    }
    Logging::LogSettings Logging::getGlobalSettings() {
        return Logger::getInstance().getGlobalSettings();
    }
    Logging::LogSettings Logging::getOutputSettings(int ID) {
        return Logger::getInstance().getOutputSettings(ID);
    }
    bool Logging::enableOutput(int ID){
        return Logger::getInstance().enableOutput(ID);
    }
    bool Logging::disableOutput(int ID){
        return Logger::getInstance().disableOutput(ID);
    }   

}

