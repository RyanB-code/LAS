#include "Display.h"

DisplayManager::DisplayManager(LoggerPtr setLogger)
    :   logger {setLogger}
{

}
DisplayManager::~DisplayManager(){
    shutdown();
}
bool DisplayManager::init(){
    if(!initGLFW())
        return false;

    if(!initImgui())
        return false;

    return true;    
}
bool DisplayManager::refresh(){

    if(!glfwWindowShouldClose(window)){
        glfwPollEvents();                   // Poll for and process events

        // Starts the Imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // MY RENDERING CODE HERE
        drawWindows();


        glClear(GL_COLOR_BUFFER_BIT);       // Does all the rendering

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);            // Swaps front and back buffers
    }
    else
        return true;    // If the window should close, return true

    return false;
}
void DisplayManager::shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
bool DisplayManager::addWindow(WindowPtr window){
    if(!window)
        return false;

    if(windows.contains(window->getID())){
        return false;
    }
    else
        return windows.emplace(window->getID(), window).second;
}
bool DisplayManager::removeWindow(uint8_t ID){
    if(!windows.contains(ID)){
        return false;
    }
    else{
        windows.erase(ID);
        return true;
    }
}
bool DisplayManager::removeWindow(LAS::Window& window){
    if(!windows.contains(window.getID())){
        return false;
    }
    else{
        windows.erase(window.getID());
        return true;
    }
}



// MARK: PRIVATE FUNCTIONS
bool DisplayManager::initGLFW(){
    if(!glfwInit()){
        logger->log("Could not initialize GLFW.", Tags{"GLFW Setup", "ERROR"});
        return false;
    }

    // Hints before createWindow
    glfwWindowHint(GLFW_FOCUSED,    GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED,  GLFW_TRUE);

    
    // Creates the window in windowed mode
    window = glfwCreateWindow(640, 480, windowTitle.c_str(), NULL, NULL);

    if(!window){
        logger->log("Could not obtain window context", Tags{"GLFW Setup", "ERROR"});
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
        logger->log("Could not initialize ImGui with OpenGL/GLFW", Tags{"ImGui Setup", "ERROR"});
        return false;
    }
    
    return true;
    
}
void DisplayManager::drawWindows(){
    if(ImGui::BeginMainMenuBar()){

        if(ImGui::BeginMenu("Windows")){

            for(auto window : windows){
                if(window.second)
                    ImGui::MenuItem(window.second->getTitle(), NULL, &window.second->shown);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    for(auto window : windows){
        if(window.second && window.second->shown)
            window.second->draw();
    }
}