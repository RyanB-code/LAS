#include "Window.h"

using namespace LAS;

Window::Window(std::string setTitle)
    :   title {setTitle}
{
    static uint8_t numberOfWindows { 0 };

    if(numberOfWindows < 0 || numberOfWindows > 200){
        throw std::out_of_range{"Cannot be more than 200 windows"};
    }

    ID = ++numberOfWindows;
}
Window::~Window(){

}
const char* Window::getTitle() const {
    return title.c_str();
}
uint8_t Window::getID() const {
    return ID;
}
void Window::draw() {
    return;
}

