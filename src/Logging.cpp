#include "Logging.h"

Log::Log(std::string setMsg, std::string setTag, std::source_location setLocation)
    :   msg {setMsg},
        location{setLocation}
{
    tags.push_back(setTag);
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




Logger::Logger(LogSettingsPtr setLogSettings) : logSettings {setLogSettings} {

}
Logger::~Logger(){

}
Log& Logger::log(std::string setMsg, const StringVector& setTag, std::source_location setLocation) const{
    // I know its bad to copy but I didnt want to deal with a LogPtr and all that jazz


    if(setTag.empty()){
        Log log{setMsg, "N/A", setLocation};

        for(std::vector<std::shared_ptr<LogOutput>>::const_iterator it {outputs.cbegin()}; it != outputs.cend(); ++it){
        it->get()->log(log, *logSettings);
    }
    }
    else{
        Log log{setMsg, setTag[0], setLocation};

        // Start at one since setTag[0] is already added
        for(int i {1}; i < setTag.size(); ++i){
            log.addTag(setTag[i]);
        }

        for(std::vector<std::shared_ptr<LogOutput>>::const_iterator it {outputs.cbegin()}; it != outputs.cend(); ++it){
        it->get()->log(log, *logSettings);
    }
    }

}
bool Logger::addOutput (std::shared_ptr<LogOutput> output){
    for(const auto& out : outputs){

        // Do not add if IDs are same
        if(out->getID() == output->getID()){
            return false;
        }
        else{
            outputs.push_back(output);
            return false;
        }
    }
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
const LogSettings& Logger::getLogSettings () const{
    return *logSettings;
}