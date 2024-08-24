#pragma once

#include <string>
#include <stdint.h>
#include <stdexcept>
#include <memory>
#include <vector>

#include <imgui/imgui.h>


namespace LAS::Windowing{
    enum class MenuOption{
        MODULE,
        UTILITY
    };

    class Window {
        public:
            Window(std::string setTitle, MenuOption setMenuOption=MenuOption::MODULE);
            virtual ~Window();

            bool shown { false };

            virtual void draw();

            const char* getTitle()      const;
            uint8_t     getID()         const;
            MenuOption  getMenuOption() const;

        protected:
            std::string title;
        private:
            uint8_t ID;
            MenuOption menuOption;
    };

    using WindowPtr     = std::shared_ptr<Window>;
    using WindowList    = std::vector<WindowPtr>;
}


