#pragma once
#include "Helpers.h"


#include <LAS/Information.h>
#include <LAS/Logging.h>
#include <GLFW/glfw3.h>

#include <map>

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

            int getLogOutputID() const;

        private:
            std::vector<Logging::Log> logHistory;
        };

    }

    class DisplayManager{
    public:
         DisplayManager() = default;
        ~DisplayManager();

        bool init       (const std::string& imGuiIniPath);
        bool refresh    (std::map<std::string, TaggedDrawFunction>& list);
        void shutdown();

        const auto& getIniPath () const { return iniPath; }
        bool saveWindowConfig   () const;

        bool addInternalWindow ( 
                const std::string& title,
                const std::string& tag, 
                std::function<void()> draw 
            );

    private:
        static constexpr char WINDOW_TITLE[]        = { "Life Application Suite" };

        std::string iniPath;
        GLFWwindow* window {nullptr};

        std::map<std::string, TaggedDrawFunction> internalWindows;

        void drawWindows(std::map<std::string, TaggedDrawFunction>& list);
    };

}


