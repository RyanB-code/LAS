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
            os << '[';
            
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
        os << std::format("{:<{}}  ", log.getMsg(), logSettings.textBoxWidth_msg);
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
LogWindow::LogWindow(const LogSettingsPtr& setLogSettings) 
    :       LAS::Window{"Log Viewer", LAS::MenuOption::UTILITY}, 
            logSettings {setLogSettings}
{

}
LogWindow::~LogWindow() {

}
void LogWindow::addLog(const Log& log){
    Log logCopy {log.getMsg(), log.getTags(), log.getLocation(), log.getTimestamp()};
    logHistory.push_back(logCopy);
}

void LogWindow::draw() {
    if(ImGui::Begin(title.c_str(), &shown)){
        ImVec2 windowSize {ImGui::GetWindowSize()};

        static bool autoScroll {true};

        ImGui::BeginChild("Options", ImVec2(windowSize.x-20, 85), ImGuiChildFlags_Border);
        ImGui::Checkbox("Show Time",            &logSettings->showTime); 
        ImGui::SameLine();
        ImGui::Checkbox("Show Tags",            &logSettings->showTags);
        ImGui::SameLine();
        ImGui::Checkbox("Show Message",         &logSettings->showMsg);
        ImGui::SameLine();
        ImGui::Checkbox("Show Code Location",   &logSettings->showLocation);
        ImGui::SameLine();
        ImGui::Checkbox("Auto Scroll",          &autoScroll);

        static int tagSizeBuffer    {logSettings->textBoxWidth_tag};
        static int msgSizeBuffer    {logSettings->textBoxWidth_msg};
        ImGui::InputInt("Tag Text Box Size", &tagSizeBuffer);
        ImGui::InputInt("Message Text Box Size", &msgSizeBuffer);
        ImGui::EndChild();

        ImGui::SeparatorText("Logs");
        ImGui::BeginChild("Logs", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
        
        if(tagSizeBuffer >= 5)
            logSettings->textBoxWidth_tag = tagSizeBuffer;
        if(msgSizeBuffer >= 20)
            logSettings->textBoxWidth_msg = msgSizeBuffer;

        for(const auto& log : logHistory){
            using namespace LAS::TextManip::Logging;
            std::ostringstream os{};    // Buffer to store formatted log

            if (logSettings->showTime)
                os << '[' << printTime(log.getTimestamp()) << "]  ";

            if (logSettings->showTags) {
                // Iterate through tags for the log and display each tag
                for (const std::string& tag : log.getTags()) {
                     os << '[';
            
                    // No Centering if too long
                    if (tag.size() >= logSettings->textBoxWidth_tag)
                        os << tag;
                    else 
                        os << std::format("{:^{}}", tag, logSettings->textBoxWidth_tag);

                    os << "] ";       
                }
                os << " ";
            }
            if (logSettings->showMsg){
                if(log.getMsg().size() > logSettings->textBoxWidth_msg)
                    os << std::format("{:^{}}...  ", log.getMsg().substr(0, logSettings->textBoxWidth_msg-3), logSettings->textBoxWidth_msg-3);
                else
                    os << std::format("{:<{}}  ", log.getMsg().substr(0, logSettings->textBoxWidth_msg), logSettings->textBoxWidth_msg);
            }
            if (logSettings->showLocation)
                os << printLocation(log.getLocation());

            ImGui::Text(os.str().c_str());
        }

        if(autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        

        ImGui::EndChild();
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
