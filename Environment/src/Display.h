#pragma once

#include "Loggers.h"

#include <LAS/Information.h>
#include <LAS/Logging.h>
#include <GLFW/glfw3.h>

#include <map>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include </usr/include/GL/gl.h>
#include </usr/include/GL/glx.h>
#include </usr/include/GL/glu.h>
#include </usr/include/GL/glext.h>
#include </usr/include/GL/glxext.h>

using namespace LAS::Logging;

namespace LAS{

    namespace Display{

        static constexpr char LOG_WINDOW_NAME[11]   { "Log Viewer" };
        static constexpr char SHELL_WINDOW_NAME[8]  { "Console" };

        struct Info {
            std::string             title { };
            std::shared_ptr<bool>   shown { };

            std::function<void()> drawFunction;
        };

        bool ensureIniExists (const std::string& path);  

        bool initGLFW(GLFWwindow** window, const std::string& title);
        bool initImgui(GLFWwindow** window, const std::string& iniPath);

        std::string makeKey (const std::string& text);

        class LogWindow : public LogOutput {
        public:
             LogWindow();
            ~LogWindow();

            void draw();
            void log(const Log& log) override;

            void setShown (std::shared_ptr<bool> set);

            int getLogOutputID() const;

        private:
            std::vector<Log> logHistory;
            std::shared_ptr<bool> shown;
        };

    }

    class DisplayManager{
    public:
         DisplayManager();
        ~DisplayManager();

        bool init(const std::string& imGuiIniPath);
        bool refresh();
        void shutdown();

        std::string getIniPath  () const;
        bool saveWindowConfig   () const;

        bool addWindow       (const std::string& title, std::function<void()> drawFunction);
        bool containsWindow  (const std::string& title) const;
        bool removeWindow    (const std::string& title);

        Display::Info& at       (const std::string& title);         // Throws the same as map::at

        std::shared_ptr<bool>   shown (const std::string& title);    

        void    closeAllWindows     () const;
        void    clearModuleWindows  ();

        std::map<std::string, Display::Info>::const_iterator cbegin() const;
        std::map<std::string, Display::Info>::const_iterator cend() const;

    private:
        static constexpr char  WINDOW_TITLE[] { "Life Application Suite" };

        std::string iniPath;
        GLFWwindow* window {nullptr};

        std::map<std::string, Display::Info> windowInformation { };

        void drawWindows();
    };

}


