#include "Console.h"

Console::Console(std::queue<std::string>& setQueue)
    :   LAS::Window("Console", LAS::MenuOption::UTILITY
    ),
        queue{setQueue}
{

}
Console::~Console(){

}
void Console::draw(){
    ImGui::Begin(title.c_str(), &shown);
    
    ImVec2 windowSize {ImGui::GetWindowSize()};
    static bool autoScroll {true};
    static bool scrollToBottom {false};

    static StringVector commandHistory;


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
        for(const auto& s : commandHistory){
            std::string msg {"$ " + s};
            ImGui::Text(msg.c_str());
        }

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
        }
        if(strBuf.empty())
            commandHistory.push_back("");                   // Hit enter to indent if they user wants without sending to command handler

        scrollToBottom = true;
        memset(inputBuf, 0, 256*(sizeof inputBuf[0]) );
        reclaimFocus = true;
    }
    if(reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();

    return;
}