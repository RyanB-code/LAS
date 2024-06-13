#include "LoggingInternal.h"

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
    using namespace LAS::TextManip::Logging;

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