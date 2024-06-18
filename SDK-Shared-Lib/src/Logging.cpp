#include <LAS/Logging.h>

// MARK: Log
Log::Log(std::string setMsg, std::string setTag, std::source_location setLocation, Timepoint setTimestamp)
    :   msg {setMsg},
        location{setLocation},
        timestamp {setTimestamp}
{
    tags.push_back(setTag);
}
Log::Log(std::string setMsg, Tags setTags, std::source_location setLocation, Timepoint setTimestamp)
     :   msg {setMsg},
         tags {setTags},         
         location{setLocation},
         timestamp {setTimestamp}
{

}

Log::~Log(){

}
std::string Log::getMsg() const{
    return msg;
}
std::source_location Log::getLocation() const {
    return location;
}
Timepoint Log::getTimestamp() const{
    return timestamp;
}
const StringVector& Log::getTags() const{
    return tags;
}
Log& Log::addTag(std::string tag){
    tags.push_back(tag);
    return *this;
}


// MARK: LogOutput

LogOutput::LogOutput() {
    static uint8_t givenIDs {0};
    ++givenIDs;
    ID = givenIDs;
}
LogOutput::~LogOutput() {

}
uint8_t LogOutput::getID () const {
    return ID;
}


// MARK: Logger

Logger::Logger(LogSettingsPtr setLogSettings) : logSettings {setLogSettings} {

}
Logger::~Logger(){

}
void Logger::log(std::string setMsg, const Tags& setTag, std::source_location setLocation) const{

    if(outputs.empty()){
        return;
    }

    // Gets Time
    const std::chrono::zoned_time time { std::chrono::current_zone(), std::chrono::system_clock::now() };

    // For a log with no tags
    if(setTag.empty()){
        Log log{setMsg, "N/A", setLocation, time};

        // Iterate through LogOutputs and output the log
        for(std::vector<std::shared_ptr<LogOutput>>::const_iterator it {outputs.cbegin()}; it != outputs.cend(); ++it){
            it->get()->log(log, *logSettings);
        }
    }
    else{
        Log log{setMsg, setTag, setLocation, time};

        // Iterate through LogOutputs and output the log
        for(std::vector<std::shared_ptr<LogOutput>>::const_iterator it {outputs.cbegin()}; it != outputs.cend(); ++it){
            it->get()->log(log, *logSettings);
        }
    }

    return;
}
bool Logger::addOutput (std::shared_ptr<LogOutput> output){
    for(const auto& out : outputs){

        // Do not add if IDs are same
        if(out->getID() == output->getID()){
            return false;
        }
        else{
            outputs.push_back(output);
            return true;
        }
    }

    // If there are no outputs, just add the parameter
    if(outputs.empty()){
        outputs.push_back(output);
        return true;
    }
    
    return false;
}
bool Logger::removeOutput (uint8_t ID){
    for(std::vector<std::shared_ptr<LogOutput>>::iterator it {outputs.begin()}; it != outputs.end();){
        if (it->get()->getID() == ID){
            it = outputs.erase(it);
            return true;
        }
        else
            ++it;
    }
    return false;
}
LogSettingsPtr Logger::getLogSettings () {
    return logSettings;
}


