#include "Logging.h"

// MARK: Log
Log::Log(std::string setMsg, std::string setTag, std::source_location setLocation, Timepoint setTimestamp)
    :   msg {setMsg},
        location{setLocation},
        timestamp {setTimestamp}
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
void Logger::log(std::string setMsg, const StringVector& setTag, std::source_location setLocation) const{

    // For a log with no tags
    if(setTag.empty()){
        Log log{setMsg, "N/A", setLocation, std::chrono::system_clock::now()+std::chrono::hours(logSettings->timeOffset)};

        // Iterate through LogOutputs and output the log
        for(std::vector<std::shared_ptr<LogOutput>>::const_iterator it {outputs.cbegin()}; it != outputs.cend(); ++it){
            it->get()->log(log, *logSettings);
        }
    }
    else{
        Log log{setMsg, setTag[0], setLocation, std::chrono::system_clock::now()+std::chrono::hours(logSettings->timeOffset)};

        // Start at one since setTag[0] is already added
        for(int i {1}; i < static_cast<int>(setTag.size()); ++i){
            log.addTag(setTag[i]);
        }
        
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
const LogSettings& Logger::getLogSettings () const{
    return *logSettings;
}

// MARK: LogToFile

LogToFile::LogToFile()
{

}
LogToFile::~LogToFile(){

}
bool LogToFile::setPath(std::string setPath){
    if(std::filesystem::exists(setPath)){
        path = setPath;
        return true;
    }
    else
        return false;
}
std::string LogToFile::getPath() const {
    return path;
}
bool LogToFile::log(const Log& log, const LogSettings& logSettings) const {
    using namespace TextManipulations::Logging;

    if(!std::filesystem::exists(path))
        return false;


    std::ostringstream os{};    // Buffer to store formatted log

    if (logSettings.showTime)
        os << '[' << printTime(log.getTimestamp()) << "]  ";

    if (logSettings.showTags) {
        // Iterate through tags for the log and display each tag
        for (const std::string& t : log.getTags()) {
            os << '[' << std::setw(logSettings.textBoxWidth_tag) << std::left;

            // No Centering if too long
            if (t.size() >= logSettings.textBoxWidth_tag)
                os << t;

            else {
                os << std::format("{:^{}}", t, logSettings.textBoxWidth_tag);
            }
            os << "]  ";           
        }
    }
    if (logSettings.showMsg) {
        std::string msg {log.getMsg()};


        // If the msg is under the max size, print normally
        if (msg.length() < logSettings.textBoxWidth_msg)
            os << std::setw(logSettings.textBoxWidth_msg) << std::left << msg;


        // If the msg is long, add spacing between next msg for readability
        else if (msg.length() >= logSettings.textBoxWidth_msg) {
            os << msg;
            if (logSettings.showLocation)
                os << std::endl << " > ";
        }
    }
    if (logSettings.showLocation)
        os << printLocation(log.getLocation());

    // Indent
    os << std::endl;

    std::ofstream file {path, std::ios_base::app};
    file << os.str();
    file.close();

    return true;
}

