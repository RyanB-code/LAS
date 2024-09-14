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

using namespace LAS::Logging;
using namespace LAS::Windowing;


namespace LAS{

    class DisplayManager;
    using DisplayManagerPtr = std::shared_ptr<DisplayManager>;

    struct WindowStatus{
        uint8_t ID;
        std::string title;
        bool open;
    };
    class DisplayManager{
    public:
        explicit DisplayManager(const LoggerPtr& setLogger);
        ~DisplayManager();

        bool init(const std::string& imGuiIniPath);
        bool refresh();
        void shutdown();

        bool addWindow      (const WindowPtr& window);
        bool removeWindow   (uint8_t ID);
        bool removeWindow   (Windowing::Window& window);

        void closeAllModuleWindows();
        void clearAllModuleWindows();

        bool        setIniPath  (const std::string& path, bool createNewFile=true); // Creates new file if not found
        std::string getIniPath  () const;

        bool saveWindowConfig   () const;

        void getAllWindowNames          (StringVector& names) const;            // Will erase all elements in parameter before writing
        void getAllWindowIDs            (std::vector<uint8_t>& IDs) const;      // Will erase all elements in parameter before writing
        uint8_t getWindowID             (const std::string& title) const;

        WindowStatus getWindowStatus    (uint8_t ID) const;

        bool setWindowShownStatus   (uint8_t ID, bool shown) const;

    private:
        const LoggerPtr&        logger;
        std::string             iniPath;

        std::map<uint8_t, WindowPtr> windows;

        GLFWwindow* window;
        std::string windowTitle { "Life Application Suite" };


        bool initGLFW();
        bool initImgui(std::string iniPath="");

        void drawWindows();

    };
}

