#include "Loggers.h"

using namespace LAS::Logging;

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
void LogToFile::log(const Log& log) {
    using namespace LAS::Logging;

    std::ostringstream os{};    // Buffer to store formatted log

    if (settings.showTime)
        os << '[' << printTime(log.timestamp) << "]  ";

    if (settings.showTags) {
        os << '[';
        os << std::format("{:^{}}", log.tag, settings.textBoxWidth_tag);
        os << "]  ";           
    }
    if (settings.showMsg) {
        os << std::format("{:<{}}  ", log.msg, settings.textBoxWidth_msg);
    }
    if (settings.showLocation)
        os << printLocation(log.location);

    // Indent
    os << std::endl;

    if(!std::filesystem::exists(path)){
        std::cerr   << "Could not find log file \"" << path << "\". The following was meant for file: \n"
                    << os.str() << "\n";
        return;
    }

    std::ofstream file {path, std::ios_base::app};
    file << os.str();
    file.close();

    return;
}



LogToConsole::LogToConsole(){

}
LogToConsole::~LogToConsole(){

}
void LogToConsole::log(const Log& log) {
    std::ostringstream os;

    if(settings.showTime)
        os << '[' << printTime(log.timestamp) << "] ";

    if(settings.showTags)
        os << std::format("[{:^{}}] ", log.tag, settings.textBoxWidth_tag);  // Centers the tag

    if(settings.showMsg)
        os << std::format("{:<{}} ", log.msg, settings.textBoxWidth_msg);  // Left-align the msg

    if(settings.showLocation)
        os << printLocation(log.location);

    std::puts(os.str().c_str());
}
