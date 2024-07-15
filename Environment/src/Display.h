#pragma once

#include <map>

#include <LAS/Window.h>
#include <LAS/Logging.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include </usr/include/GL/gl.h>
#include </usr/include/GL/glx.h>
#include </usr/include/GL/glu.h>
#include </usr/include/GL/glext.h>
#include </usr/include/GL/glxext.h>


namespace LAS{

    class DisplayManager;
    using DisplayManagerPtr = std::shared_ptr<DisplayManager>;  
    class DisplayManager{
    public:
        explicit DisplayManager(const LoggerPtr& setLogger);
        ~DisplayManager();

        bool init();
        bool refresh();
        void shutdown();

        bool addWindow      (const WindowPtr& window);
        bool removeWindow   (uint8_t ID);
        bool removeWindow   (LAS::Window& window);

    private:
        const LoggerPtr&      logger;

        std::map<uint8_t, WindowPtr> windows;

        GLFWwindow* window;
        std::string windowTitle { "Life Application Suite" };


        bool initGLFW();
        bool initImgui();

        void drawWindows();

    };
}

