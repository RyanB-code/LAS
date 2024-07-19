#include "Shell.h"


using namespace LAS;

// MARK: Shell Output
ShellOutput::ShellOutput()
{
    static uint8_t givenIDs {0};

    if(givenIDs < 0 || givenIDs > 200){
        throw std::out_of_range{"Cannot be more than 200 ShellOutputs"};
    }
    ID = ++givenIDs;
}
ShellOutput::~ShellOutput(){

}
uint8_t ShellOutput::getID() const{
    return ID;
}

// MARK: ConsoleWindow
ConsoleWindow::ConsoleWindow(std::queue<std::string>& setQueue)
    :   Window("Console", MenuOption::UTILITY),
        queue{setQueue}
{

}
ConsoleWindow::~ConsoleWindow(){

}
void ConsoleWindow::draw(){
    ImGui::Begin(title.c_str(), &shown);
    
    ImVec2 windowSize           {ImGui::GetWindowSize()};
    static bool autoScroll      {true};
    static bool scrollToBottom  {false};


    if(ImGui::BeginChild("Options", ImVec2(windowSize.x-20, 40), ImGuiChildFlags_Border)){
        static bool showDemo {false};
        ImGui::Checkbox("Show Demo Window", &showDemo);
        if(showDemo)
            ImGui::ShowDemoWindow();

        ImGui::SameLine();
        ImGui::Checkbox("Auto Scroll", &autoScroll);
        ImGui::EndChild();
    }
    ImGui::SeparatorText("Console");

    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
       
       ImGui::TextUnformatted(textHistory.str().c_str());

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom  = false;
    }
    ImGui::EndChild();
    ImGui::Separator();

    static char inputBuf [256];
    bool reclaimFocus {false};
    ImGuiInputTextFlags inputBoxflags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputBoxflags)){

        std::string strBuf{inputBuf};
        if(!strBuf.empty() && !strBuf.starts_with(' ')){
            commandHistory.push_back(strBuf);
            queue.push(strBuf);                             // Push to command queue

            // Add to textHistory
            textHistory << "$ " << strBuf << "\n";
        }
        if(strBuf.empty())
            textHistory<<("\n");                   // Hit enter to indent if they user wants without sending to command handler

        scrollToBottom = true;
        memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );
        reclaimFocus = true;
    }
    if(reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();

    return;
}
void ConsoleWindow::output(const std::ostringstream& os) {
    textHistory << os.str();
    return;
}


// MARK: Shell
Shell::Shell(std::shared_ptr<ConsoleWindow> setWindow)
    : window{setWindow}
{
    if(!window){
        window = std::shared_ptr<ConsoleWindow>{new ConsoleWindow{commandQueue}};
    }
}
Shell::~Shell(){

}
bool Shell::addCommand(CommandPtr command){
    if(commands.contains(command->getKey())){
        return false;
    }
    else{
        commands.try_emplace(command->getKey(), std::move(command));
        return true;
    }
    return false;
}
bool Shell::addOutput(const ShellOutputPtr& output){
    for(const auto& o : outputs){
        // Do not add if IDs are same
        if(o->getID() == output->getID()){
            return false;
        }
        else{
            outputs.push_back(std::move(output));
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
bool Shell::removeOutput(const uint8_t& ID){
    for(std::vector<ShellOutputPtr>::iterator it {outputs.begin()}; it != outputs.end();){
        if (it->get()->getID() == ID){
            it = outputs.erase(it);
            return true;
        }
        else
            ++it;
    }
    return false;
}
void Shell::addToQueue(const std::string& entry){
    if(!entry.empty())
        commandQueue.push(entry);

    return;
}
bool Shell::handleCommandQueue(){
    for (/*Nothing*/; !commandQueue.empty(); commandQueue.pop()){

        // Buffers
        std::vector<std::string> arguments;
        std::stringstream inputStream{};
        
        inputStream << commandQueue.front();            // Read the string
        std::string buffer;
        while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
            arguments.push_back(buffer);                // Add the quoted token or separated by space token
        }

        // Initial lookup of first arg only to see if it is a valid command
        if(commands.contains(arguments[0])){
            std::string command {arguments[0]};
            arguments.erase(arguments.begin());  // Removes the first command key (from arg list)

            // Execute command and pass to every output
            for(const auto& output : outputs){
                output->output(commands.at(command)->execute(arguments).second);
            }
        }
        else{
            for(const auto& output : outputs){
                std::ostringstream msg {"Command \"" + arguments[0] + "\" not found\n"};
                output->output(msg);
            }
        }
    }

    return true;   
}
bool Shell::readRCFile(const std::string& path){

     if(std::filesystem::exists(path)){
        std::ifstream settingsFile {path}; 
        std::string line;

        // Read line by line
        while (std::getline(settingsFile, line)){
            std::stringstream inputStream{};
            inputStream << line;
            std::string buffer {inputStream.str()};

            // Filter out comments, newlines, and empty lines. Add the rest to commandQueue
            if(!buffer.starts_with("#") && !buffer.starts_with('\n') && !buffer.empty())
                commandQueue.push(buffer);
        }
        return true;
    }
    else{
        std::ofstream newSettings {path, std::ios::trunc};

        if(!std::filesystem::exists(path)){
            std::cerr << "Could not create new configuration file at [" << path << "]\n";
            return false;
        }

        newSettings << "# Life Application Suite Configuration File\n# Any commands entered here will be executed upon each startup" << std::endl;
        newSettings.close();
        return true;
    }

    return false;
}
const std::unordered_map<std::string, CommandPtr>& Shell::viewCommandInfo(){
    return commands;
}
std::shared_ptr<ConsoleWindow> Shell::getWindow(){
    return window;
}