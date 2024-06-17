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

// MARK: Log Window
LogWindow::LogWindow(const LogSettings& setLogSettings) :   LAS::Window{"Log Viewer"}, logSettings {setLogSettings}
{

}
LogWindow::~LogWindow() {

}
void LogWindow::addLog(const Log& log){
    Log logCopy {log.getMsg(), log.getTags().front(), log.getLocation(), log.getTimestamp()};
    logHistory.push_back(logCopy);
}

void LogWindow::draw() {
    if(ImGui::Begin(title.c_str(), &shown)){

        for(auto log : logHistory){
            using namespace LAS::TextManip::Logging;
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

            ImGui::Text(os.str().c_str());
        }
    }
    ImGui::End();
}


//MARK: Log To Window
LogToWindow::LogToWindow(std::shared_ptr<LogWindow> setWindow)
    :   window{setWindow}
{

}
LogToWindow::~LogToWindow(){

}
bool LogToWindow::log(const Log& log, const LogSettings& logSettings) const {
    window->addLog(log);
    return true;
}
std::shared_ptr<LogWindow> LogToWindow::getWindow(){
    return window;
}
