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

    ImGui::Text("In console");


    ImGui::End();
    return;
}