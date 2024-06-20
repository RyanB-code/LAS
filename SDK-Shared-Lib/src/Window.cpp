#include <LAS/Window.h>

using namespace LAS;

Window::Window(std::string setTitle, MenuOption setMenuOption)
    :   title {setTitle},
        menuOption {setMenuOption}
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
MenuOption Window::getMenuOption() const {
    return menuOption;
}
void Window::draw() {
    return;
}

