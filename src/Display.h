#include "Logging.h"
#include "ModuleManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include </usr/include/GL/gl.h>
#include </usr/include/GL/glx.h>
#include </usr/include/GL/glu.h>
#include </usr/include/GL/glext.h>
#include </usr/include/GL/glxext.h>

class DisplayManager;

using DisplayManagerPtr = std::shared_ptr<DisplayManager>;

class DisplayManager{
public:
    DisplayManager(const Logger& setLogger, ModuleManagerPtr setModuleManager);
    ~DisplayManager();

    bool init();
    bool refresh();
    void shutdown();

private:
    bool initGLFW();
    bool initImgui();

    const Logger& logger;
    ModuleManagerPtr moduleManager;

    GLFWwindow* window;
    std::string windowTitle { "Life Application Suite" };
};


namespace LAS_Display{
    void setupWindow(std::string title);
}
