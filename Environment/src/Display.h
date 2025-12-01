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

namespace LAS{

    namespace Display{

        static constexpr char LOG_WINDOW_NAME[]     = { "Log Viewer" };
        static constexpr char SHELL_WINDOW_NAME[]   = { "Console" };

        struct ModuleDraw {
            std::string             title { };
            std::string             tag   { };  
            std::shared_ptr<bool>   shown { };

            std::function<void()> drawFunction;
        };

        bool ensureIniExists (const std::string& path);  

        bool initGLFW(GLFWwindow** window, const std::string& title);
        bool initImgui(GLFWwindow** window, const std::string& iniPath);

        std::string makeKey (const std::string& text);

        class LogWindow : public Logging::LogOutput {
        public:
             LogWindow();
            ~LogWindow() = default;

            void draw();
            void log(const Logging::Log& log) override;

            void setShown (std::shared_ptr<bool> set);

            int getLogOutputID() const;

        private:
            std::vector<Logging::Log> logHistory;
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

        bool addWindow       (const std::string& title, const std::string& tag, std::function<void()> drawFunction);
        bool containsWindow  (const std::string& title) const;
        bool removeWindow    (const std::string& title);

        Display::ModuleDraw& at       (const std::string& title);         // Throws the same as map::at

        std::shared_ptr<bool>   shown (const std::string& title);    

        void    closeAllWindows     () const;
        void    clearModuleWindows  ();

        std::map<std::string, Display::ModuleDraw>::const_iterator cbegin() const;
        std::map<std::string, Display::ModuleDraw>::const_iterator cend() const;

    private:
        static constexpr char  WINDOW_TITLE[] { "Life Application Suite" };

        std::string iniPath;
        GLFWwindow* window {nullptr};

        std::map<std::string, Display::ModuleDraw> windowInformation { };

        void drawWindows();
    };

}


