#include "Shell.h"


using namespace LAS;

void Shell::draw(){
    ImVec2 windowSize           {ImGui::GetWindowSize()};
    static bool autoScroll      {true};
    static bool scrollToBottom  {false};


    if(ImGui::BeginChild("Options", ImVec2(windowSize.x-20, 40), ImGuiChildFlags_Border)){
        if(ImGui::Button("Clear", ImVec2{50,20}))
            consoleTextBuffer.clear();

        ImGui::SameLine();
        ImGui::Checkbox("Auto Scroll", &autoScroll);

        ImGui::SameLine();
        static bool showDemo {false};
        ImGui::Checkbox("Show Demo Window", &showDemo);
        if(showDemo)
            ImGui::ShowDemoWindow();
    }
    ImGui::EndChild();

    ImGui::SeparatorText("Console");

    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {

        consoleTextBuffer.writeToScreen();

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom  = false;
        ImGui::EndChild();
    }

    bool reclaimTextBoxFocus {false};
    static char         inputBuf[256];              // Holds the buffer for inputting commands
    static std::string  inputBeforeHistoryScroll;   // Holds input that was in the inpuBuf before scrolling

    // Scroll command history
    static size_t   offsetFromEnd           { 0 };
    static bool     fetchHistory            {false}; 

    if(ImGui::IsWindowFocused()){
       if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)){
            if(offsetFromEnd == 0)
                inputBeforeHistoryScroll = inputBuf;

            if(offsetFromEnd < 256)
                ++offsetFromEnd;    

            fetchHistory = true;
        }
        if(ImGui::IsKeyPressed(ImGuiKey_DownArrow)){
            if(offsetFromEnd > 0)
                --offsetFromEnd;
            
            if(offsetFromEnd == 0){                                     // Command history index reached the end
                ImGui::ClearActiveID();                                 // Clears the inputText field to accept input

                if(inputBeforeHistoryScroll.empty())
                    memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );                             // Clears the input buffer
                else
                    inputBeforeHistoryScroll.copy(inputBuf, inputBeforeHistoryScroll.length()); // Sets inputBuf to previous inputBeforeHistoryScroll

                reclaimTextBoxFocus = true;
            }
            fetchHistory = true;
        }

        if(fetchHistory){
            size_t sizeOfCommandHistory { commandHistory.size() };
            
            // Write to text box
            if(offsetFromEnd > 0 && offsetFromEnd <= sizeOfCommandHistory){
                ImGui::ClearActiveID();                             // Clears the inputText field to accept input
                memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );     // Clears the input buffer

                size_t commandIndex {sizeOfCommandHistory - offsetFromEnd};
                
                std::string stringCommandBuf {commandHistory[commandIndex]};    // Retrieves the command in the list of command history
                stringCommandBuf.copy(inputBuf, stringCommandBuf.length());     // Set input buffer
            }
            reclaimTextBoxFocus = true;
            fetchHistory = false;
        }
    }


    // Handle input box
    ImGuiInputTextFlags inputBoxflags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), inputBoxflags)){

        std::string strBuf{inputBuf};
        if(!strBuf.empty() && !strBuf.starts_with(' ')){
            commandHistory.push_back(strBuf);
            addToQueue(strBuf);                             // Push to command queue

            // Add to textHistory
            consoleTextBuffer.push("$ " + strBuf + '\n');
        }
        if(strBuf.empty())
            consoleTextBuffer.push("\n");                            // Hit enter to indent if they user wants without sending to command handler

        inputBeforeHistoryScroll    = "";                   // Resets saved inputBuf for history
        scrollToBottom              = true;
        reclaimTextBoxFocus         = true;
        offsetFromEnd               = 0;                    // Entering a new command resets the place of the history index

        ImGui::ClearActiveID();                             // Clears the inputText field to accept input
        memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );     // Reset input buffer

    }
    ImGui::SetItemDefaultFocus();   // Sets the input box as the item to focus on when opening the window

    if(reclaimTextBoxFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
}
bool Shell::addCommandGroup(const std::string& name){
    if(commands.contains(name))
        return false;
  
    return commands.try_emplace(name, std::unordered_map<std::string, std::shared_ptr<Command>>{}).second;
}
void Shell::removeCommandGroup (const std::string& name){
    commands.erase(name);
}
const std::unordered_map<std::string, std::shared_ptr<Command>>& Shell::getGroup (const std::string& name) const{
    if(!commands.contains(name))
        throw std::out_of_range{"No command group\"" + name + "\" found."};
    
    return 
        commands.at(name);
}
bool Shell::addCommand(const std::string& groupName, std::shared_ptr<Command> command){
    if(!commands.contains(groupName) || !command)
        return false;
    
    try{
        auto& group {commands.at(groupName)};

        // Check the actual group before adding
        if(group.contains(command->getKey()))
            return false;

        group.try_emplace(command->getKey(), command);
        return true;
    }
    catch(std::out_of_range& e){
        return false;               // If the group could not be found
    }
}
bool Shell::addToCommandHistory(const std::string& text){
    commandHistory.emplace_back(text);
    return true;
}
bool Shell::getAllGroupsManuals(std::ostringstream& os) const {
    for(const auto& group : commands){
        os << group.first << " Manual:\n";
        getGroupManual(os, group.first);
    }
    return true;
}
bool Shell::getGroupManual(std::ostringstream& os, const std::string& groupName) const{
    if(!commands.contains(groupName))
        return false;

    // 32 spaces is the size of 2 tabs and 20 chars
    static const std::string padding {"                                "};

    for(auto& [key, command] : getGroup(groupName)){
        std::string description {command->getDescription()};
        size_t charNum { 0 };

        while(charNum < description.size()){
            char& c {description[charNum]};
            if(c == '\n'){
                description.insert(charNum+1, padding);
                charNum += padding.size();
            }
            ++charNum;
        }

        os << std::format("\t{:20}\t", command->getKey());
        os << std::format("\t{}\n", description);
        
    }

    return true;
}


bool Shell::addAlias(const std::string& key, const std::string& value){
    if(aliases.contains(key))
        return false;
    
    return aliases.try_emplace(key, value).second;
}
bool Shell::removeAlias(const std::string& key){
    if(aliases.contains(key)){
        aliases.erase(key);
        return true;
    }
    else
        return false;
}
std::string Shell::findAlias(const std::string& key){
    if(aliases.contains(key)){
        return aliases[key];
    }

    return "";
}
void Shell::removeAllAliases(){
    aliases.erase(aliases.begin(), aliases.end());
}



void Shell::addToQueue(const std::string& entry){
    if(!entry.empty())
        commandQueue.push(entry);

    return;
}
bool Shell::handleCommandQueue(bool writeToHistory){
    for (/*Nothing*/; !commandQueue.empty(); commandQueue.pop()){

        // Buffer variables
        std::vector<std::string>    arguments       { };
        std::stringstream           inputStream     { };
        bool                        executeCommand  { true };
        
        inputStream << commandQueue.front();            // Read the string


        std::string rawInput {inputStream.str()};
        if(writeToHistory)
            LAS::ShellHelper::writeToCommandHistory(commandHistoryPath, rawInput); // Write to command history file

        // Create alias if in proper format
        if(rawInput.size() >= 5 && rawInput.substr(0,6) == "alias "){ // Need the space in there
            executeCommand = false;

            try{
                auto alias {ShellHelper::createAlias(rawInput.substr(6, rawInput.size()))};
                
                if(!addAlias(alias.first, alias.second))
                    consoleTextBuffer.push("Could not add alias \"" + alias.first + "\".\nAn alias for that key may already exist.\n");
            }
            catch(std::invalid_argument& e){
                consoleTextBuffer.push(std::string{e.what()} + "\nAlias is not in correct format.\n");
            }
        }

        if(executeCommand){
             // Replace if alias was found
            std::string aliasValue {findAlias(rawInput)};
            if(aliasValue != "")
                inputStream.str(aliasValue);

            // Parse the line
            std::string buffer;
            while (inputStream >> std::quoted(buffer)) {    // Separate by quotes or spaces
                arguments.push_back(buffer);                // Add the quoted token or separated by space token
            }

            // Ensure at least 2 arguments
            if(arguments.size() < 2){
                consoleTextBuffer.push("Invalid number of arguments.\n");
                commandQueue.pop();
                break;
            }

            // Parse and execute the command in the respective command group
            std::string firstArg {arguments[0]};
            try{
                auto& group  {getGroup(firstArg) };     // Sets the group
                arguments.erase(arguments.begin());     // Removes the group from arg list

                std::string commandString {arguments [0]};    // Contains first actual command to be run
                arguments.erase(arguments.begin());     // Removes the command key from arg list, leaving just the arguments

                // Verify command exists
                if(group.contains(commandString))
                    consoleTextBuffer.push(group.at(commandString)->execute(arguments).msg.str());   // Execute command and pass to every output
                else
                    consoleTextBuffer.push("Command \"" + commandString + "\" not found in group \"" + firstArg + "\".\n");
            }
            catch(std::out_of_range& e){
                consoleTextBuffer.push("Could not find command group \"" + firstArg + "\".\n");
            }
        } // End of if(!aliasCreated)
    }

    return true;   
}


std::string Shell::getRCPath() const{
    return rcPath;
}
std::string Shell::getCommandHistoryPath() const{
    return commandHistoryPath;
}
bool Shell::setRCPath (const std::string& path, bool createNewFile){
    if(std::filesystem::exists(path)){
        rcPath = path;
        return true;
    }

    if(!createNewFile)
        return false;
   
    std::ofstream newRCFile {path, std::ios::trunc};

    if(!std::filesystem::exists(path))
        return false;

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
bool Shell::readRCFile(std::string path){
    if(path.empty())
        path = getRCPath();
    
    return ShellHelper::readRCFile(path, commandQueue);
}




// MARK: Shell Helper
namespace LAS::ShellHelper{
    bool readRCFile(const std::string& path, std::queue<std::string>& queue){

        if(!std::filesystem::exists(path))
            return false;

        std::ifstream rcFile {path}; 
        std::string line;

        // Read line by line
        while (std::getline(rcFile, line)){
            std::stringstream inputStream{};
            inputStream << line;
            std::string buffer {inputStream.str()};

            // Filter out comments, newlines, and empty lines. Add the rest to commandQueue
            if(!buffer.starts_with("#") && !buffer.starts_with('\n') && !buffer.empty())
                queue.push(buffer);
        }

        return true;
    }
    bool defaultInitializeRCFile(const std::string& path){
        std::ofstream newRCFile {path, std::ios::trunc};

        if(!std::filesystem::exists(path))
            return false;

        std::ofstream rcFile {path, std::ios::trunc};

        rcFile <<   "# Life Application Suite Configuration File\n"
                    "# Any commands entered here will be executed upon each startup\n";

        rcFile <<   "# Added by default\n"
                    "alias man=\"las man\"\n"; 

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
    std::pair<std::string, std::string> createAlias(std::string text){
        if(text.find('=') == std::string::npos){
            throw std::invalid_argument("Does not contain '=' sign.");
        }

        try {
            std::stringstream   rawKeyBuffer    { text.substr(0, text.find('='))};
            std::stringstream   rawValueBuffer  { text.substr(text.find('=')+1, text.size())};

            std::string formattedKey;
            rawKeyBuffer >> std::quoted(formattedKey);

            std::string formattedValue;
            rawValueBuffer >> std::quoted(formattedValue);
        
            if(formattedKey.contains(' '))
                throw std::invalid_argument ("Key cannot contain spaces.");

            return std::pair<std::string, std::string>{formattedKey, formattedValue};
        }
        catch (std::out_of_range& e){
            throw std::invalid_argument("Not in correct format.");
        }
    }


}
