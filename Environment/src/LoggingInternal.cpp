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
void LogToFile::log(const Log& log) const {
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
LogWindow::LogWindow(LogSettings& setSettings) : Windowing::Window{"Log Viewer", Windowing::MenuOption::UTILITY}, settings {setSettings} {

}
LogWindow::~LogWindow() {

}
void LogWindow::addLog(const Log& log){
    Log logCopy {log.msg, log.tag, log.location, log.timestamp};
    logHistory.push_back(logCopy);
}

void LogWindow::draw() {

    if(!ImGui::Begin(title.c_str(), &shown)){
        ImGui::End();
        return;
    }

    ImVec2 windowSize {ImGui::GetWindowSize()};

    static bool autoScroll      {true};
    static int  tagSizeBuffer   {settings.textBoxWidth_tag};
    static int  msgSizeBuffer   {settings.textBoxWidth_msg};

    ImGui::BeginChild("Options", ImVec2(windowSize.x-20, 85), ImGuiChildFlags_Border);
    ImGui::Checkbox("Show Time",            &settings.showTime); 
    ImGui::SameLine();
    ImGui::Checkbox("Show Tags",            &settings.showTags);
    ImGui::SameLine();
    ImGui::Checkbox("Show Message",         &settings.showMsg);
    ImGui::SameLine();
    ImGui::Checkbox("Show Code Location",   &settings.showLocation);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scroll",          &autoScroll);


    if(ImGui::InputInt("Tag Text Box Size",     &tagSizeBuffer, 1, 5, ImGuiInputTextFlags_EnterReturnsTrue))
        settings.textBoxWidth_tag = tagSizeBuffer;
    if(ImGui::InputInt("Message Text Box Size", &msgSizeBuffer, 1, 5, ImGuiInputTextFlags_EnterReturnsTrue))
        settings.textBoxWidth_msg = msgSizeBuffer;
    ImGui::EndChild();


    // Ensure sizes are not too small
    if(tagSizeBuffer >= 5)
        settings.textBoxWidth_tag = tagSizeBuffer;
    else{
        settings.textBoxWidth_tag = 5;
        tagSizeBuffer = 5;
    }
    if(msgSizeBuffer >= 20)
        settings.textBoxWidth_msg = msgSizeBuffer;
    else{
        settings.textBoxWidth_msg = 20;
        msgSizeBuffer = 20;
    }

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
                os << std::format("{:<{}}  ", log.msg.substr(0, settings.textBoxWidth_msg), settings.textBoxWidth_msg);
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


//MARK: Log To Window
LogToWindow::LogToWindow()
    : window{std::make_shared<LogWindow>(LogWindow{settings})}
{

}
LogToWindow::~LogToWindow(){

}
void LogToWindow::log(const Log& log) const {
    window->addLog(log);
    return;
}
std::shared_ptr<LogWindow> LogToWindow::getWindow(){
    return window;
}
