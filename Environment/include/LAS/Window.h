#pragma once

#include <string>
#include <stdint.h>
#include <stdexcept>
#include <memory>
#include <vector>

#include <imgui/imgui.h>


namespace LAS{

    class Window {
        public:
            Window(std::string setTitle);
            virtual ~Window();

            bool shown { false };

            virtual void draw();

            const char* getTitle() const;
            uint8_t     getID() const;

        protected:
            std::string title;
        private:
            uint8_t ID;
    };
}

using WindowPtr = std::shared_ptr<LAS::Window>;
using WindowList = std::vector<WindowPtr>;
