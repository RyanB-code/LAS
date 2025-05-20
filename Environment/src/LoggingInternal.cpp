#include "LoggingInternal.h"

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

// MARK: Log Window
LogWindow::LogWindow() : Windowing::Window{"Log Viewer", Windowing::MenuOption::UTILITY}, LogOutput{ } {

}
LogWindow::~LogWindow() {

}
int LogWindow::getWindowID() const{
    return Window::getID();
}
int LogWindow::getLogOutputID() const {
    return LogOutput::getID();
}
void LogWindow::log(const Log& log) {
    Log logCopy {log.msg, log.tag, log.location, log.timestamp};
    logHistory.push_back(logCopy);
}

void LogWindow::draw() {
    if(!ImGui::Begin(title.c_str(), &shown)){
        ImGui::End();
        return;
    }

    ImGui::BeginChild("Options", ImVec2(ImGui::GetWindowSize().x-20, 85), ImGuiChildFlags_Border);
    static bool autoScroll      { true };
    ImGui::Checkbox("Show Time",            &settings.showTime); 
    ImGui::SameLine();
    ImGui::Checkbox("Show Tags",            &settings.showTags);
    ImGui::SameLine();
    ImGui::Checkbox("Show Message",         &settings.showMsg);
    ImGui::SameLine();
    ImGui::Checkbox("Show Code Location",   &settings.showLocation);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scroll",          &autoScroll);

    // Ensure sizes are not too small
    if(settings.textBoxWidth_tag < 5)
        settings.textBoxWidth_tag = 5;
    if(settings.textBoxWidth_msg < 20)
        settings.textBoxWidth_msg = 20;


    ImGui::InputInt("Tag Text Box Size",     &settings.textBoxWidth_tag, 1, 5);
    ImGui::InputInt("Message Text Box Size", &settings.textBoxWidth_msg, 1, 5);
    ImGui::EndChild(); // End Options Child
    
    

    // Log window portion
    ImGui::SeparatorText("Logs");
    ImGui::BeginChild("Logs", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

    for(const auto& log : logHistory){
        using namespace LAS::Logging;
        std::ostringstream os{};    // Buffer to store formatted log

        if (settings.showTime)
            os << '[' << printTime(log.timestamp) << "]  ";

        if (settings.showTags) {
            os << '[';
            os << std::format("{:^{}}", log.tag, settings.textBoxWidth_tag);
            os << "] ";       
     
            os << " ";
        }
        if (settings.showMsg){
            if(log.msg.size() > settings.textBoxWidth_msg)
                os << std::format("{:^{}}...  ", log.msg.substr(0, settings.textBoxWidth_msg-3), settings.textBoxWidth_msg-3);
            else
                os << std::format("{:<{}}  ", log.msg, settings.textBoxWidth_msg);
        }
        if (settings.showLocation)
            os << printLocation(log.location);

        ImGui::TextUnformatted(os.str().c_str());
    }

    if(autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    

    ImGui::EndChild();
    ImGui::End();
}

// Log To Console
LogToConsole::LogToConsole(){

}
LogToConsole::~LogToConsole(){

}
void LogToConsole::log(const Log& log) const {
    std::ostringstream os;

    if(settings.showTime)
        os << '[' << printTime(log.timestamp) << "] ";

    if(settings.showTags)
        os << std::format("[{:^{}}] ", log.tag, settings.textWidth_tag);  // Centers the tag

    if(settings.showMsg)
        os << std::format("{:<{}} ", log.msg, settings.textWidth_msg);  // Left-align the msg

    if(settings.showLocation)
        os << printLocation(log.location);

    std::puts(os.str().c_str());
}
