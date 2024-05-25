#include "Display.h"

DisplayManager::DisplayManager(const Logger& setLogger, ModuleManagerPtr setModuleManager)
    :   logger {setLogger},
        moduleManager {setModuleManager}
{

}
DisplayManager::~DisplayManager(){

}
bool DisplayManager::init(){
    if(!initGLFW())
        return false;

    if(!initImgui())
        return false;

    return true;    
}
bool DisplayManager::refresh(){
    
    // Loop until wndow closes
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();                   // Poll for and process events

        // Starts the Imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // My rendering here
        // SUDOCODE
        // 1. Iterate over modules and refresh
        LAS_Display::setupWindow(windowTitle);


        glClear(GL_COLOR_BUFFER_BIT);       // Does all the rendering

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);      // Swaps front and back buffers
    }

    return true;
}
void DisplayManager::shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

}
bool DisplayManager::initGLFW(){
    if(!glfwInit()){
        logger.log("Could not initialize GLFW.", Tags{"GLFW", "ERROR"});
        return false;
    }

    // Hints before createWindow
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    
    // Creates the window in windowed mode
    window = glfwCreateWindow(640, 480, windowTitle.c_str(), NULL, NULL);

    if(!window){
        logger.log("Could not obtain window contect", Tags{"GLFW", "ERROR"});
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    

    return true;
}
bool DisplayManager::initImgui(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends

    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    if(!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
            !ImGui_ImplOpenGL3_Init() )
    {
        logger.log("Could not initialize ImGui with OpenGL/GLFW", Tags{"ImGui", "ERROR"});
        return false;
    }
    
    return true;
    
}

// MARK: LAS Display Namespace
namespace LAS_Display{
    void setupWindow(std::string title){
        static bool shown {true};

        ImGui::Begin(title.c_str(), &shown, ImGuiWindowFlags_MenuBar);

        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Testing")){
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Menu Bar Setup
        if(ImGui::BeginMenuBar()){
            if(ImGui::BeginMenu("Test")){
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
}