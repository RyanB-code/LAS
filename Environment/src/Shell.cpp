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

    static char inputBuf[256];

    // Scroll command history
    static size_t   offsetFromEnd           { 0 };
    static bool     fetchHistory            {false}; 

    if(ImGui::IsWindowFocused()){
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))){
            if(offsetFromEnd < 256)
                ++offsetFromEnd;    
            fetchHistory = true;
        }

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))){
            if(offsetFromEnd > 0)
                --offsetFromEnd;
            fetchHistory = true;
        }

        // Use of arrow key detected
        if(fetchHistory){
            size_t sizeOfCommandHistory { commandHistory.size() };

            if(offsetFromEnd < 0)
                offsetFromEnd = 0;
            else if(offsetFromEnd > sizeOfCommandHistory)
                offsetFromEnd = sizeOfCommandHistory;

            if(offsetFromEnd > 0 && offsetFromEnd <= sizeOfCommandHistory){
                ImGui::ClearActiveID();                             // Clears the inputText field to accecpt input
                memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );     // Clears the input buffer

                size_t commandIndex {sizeOfCommandHistory - offsetFromEnd};
                
                std::string stringCommandBuf {commandHistory[commandIndex]};  
                stringCommandBuf.copy(inputBuf, stringCommandBuf.length());     // Set input buffer
            }
            ImGui::SetKeyboardFocusHere(-1);
            fetchHistory = false;
        }
    }


    // Handle input box
    bool reclaimFocus {false};
    ImGuiInputTextFlags inputBoxflags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputBoxflags)){

        std::string strBuf{inputBuf};
        if(!strBuf.empty() && !strBuf.starts_with(' ')){
            commandHistory.push_back(strBuf);
            queue.push(strBuf);                             // Push to command queue

            // Add to textHistory
            textHistory << "$ " << strBuf << '\n';
        }
        if(strBuf.empty())
            textHistory<<('\n');                            // Hit enter to indent if they user wants without sending to command handler

        scrollToBottom = true;
        memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );
        reclaimFocus = true;
        offsetFromEnd = 0;                                  // Entering a new command resets the place
    }

    ImGui::SetItemDefaultFocus();
    if(reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();

    return;
}
void ConsoleWindow::output(const std::ostringstream& os) {
    textHistory << os.str();
    return;
}
bool ConsoleWindow::addToCommandHistory (const std::string& text){
    commandHistory.push_back(text);
    return true;
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

            LAS::ShellHelper::writeToCommandHistory(commandHistoryPath, inputStream.str()); // Write to command history file

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
        std::ifstream rcFile {path}; 
        std::string line;

        // Read line by line
        while (std::getline(rcFile, line)){
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
        std::ofstream newRCFile {path, std::ios::trunc};

        if(!std::filesystem::exists(path)){
            std::cerr << "Could not create new configuration file at [" << path << "]\n";
            return false;
        }
        rcPath = path;
        return ShellHelper::defaultInitializeRCFile(rcPath);
    }

    return false;
}
const std::unordered_map<std::string, CommandPtr>& Shell::viewCommandInfo(){
    return commands;
}
std::shared_ptr<ConsoleWindow> Shell::getWindow() const{
    return window;
}
std::string Shell::getRCPath() const{
    return rcPath;
}
std::string Shell::getCommandHistoryPath() const{
    return commandHistoryPath;
}
bool Shell::setRCPath (const std::string& path){
    if(std::filesystem::exists(path)){
        rcPath = path;
        return true;
    }

    std::ofstream newRCFile {path, std::ios::trunc};

    if(!std::filesystem::exists(path)){
        std::cerr << "Could not create new configuration file at [" << path << "]\n";
        return false;
    }

    rcPath = path;
    return ShellHelper::defaultInitializeRCFile(rcPath);
}
bool Shell::setCommandHistoryPath (const std::string& path){
    if(std::filesystem::exists(path)){
        commandHistoryPath = path;
        return true;
    }

    std::ofstream newRCFile {path, std::ios::trunc};

    if(!std::filesystem::exists(path)){
        std::cerr << "Could not create new command history file at [" << path << "]\n";
        return false;
    }

    commandHistoryPath = path;
    return true;
}

// MARK: Shell Helper
namespace LAS::ShellHelper{
    bool defaultInitializeRCFile(const std::string& path){
        if(!std::filesystem::exists(path))
            return false;
        
        std::ofstream rcFile {path, std::ios::trunc};

        rcFile << "# Life Application Suite Configuration File\n# Any commands entered here will be executed upon each startup" << std::endl;
        rcFile.close();

        return true;
    }
    bool writeToCommandHistory  (const std::string& path, const std::string& text){
        if(!std::filesystem::exists(path))
            return false;

        std::ofstream file (path, std::ios_base::app);
        
        if(text.back() != '\n')
            file << text << '\n';
        else
            file << text;
        
        return true;
    }
    uint16_t linesInFile (const std::string& path){
        if(!std::filesystem::exists(path))
            return 0;

        std::ifstream file (path, std::ios_base::app);
        uint16_t linesInFile { 0 };

        std::string lastLineRead;
        if(file.is_open()) {
            while (getline(file, lastLineRead)) {
                ++linesInFile;
            }
        }

        file.close();
        return linesInFile;
    }
    bool retrieveLines (const std::string& path, StringVector& cache, uint16_t cacheNumberOfLines){
        if(!std::filesystem::exists(path))
            return false;

        uint16_t totalLines {linesInFile(path)};
        
        if(cacheNumberOfLines > totalLines)
            cacheNumberOfLines = totalLines;
    
        uint16_t linesToCache = (uint16_t)totalLines - (uint16_t)cacheNumberOfLines;
        

        std::ifstream file (path, std::ios_base::app);
        uint16_t currentLine {0};

        std::string lastLineRead;
        if(file.is_open()) {
            while (getline(file, lastLineRead)) {
                if(currentLine >= linesToCache && currentLine <= totalLines)
                    cache.push_back(lastLineRead);
                ++currentLine;
                lastLineRead = "";
            }
        }

        file.close();
        return true;
    }

}
