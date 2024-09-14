#include "Display.h"

using namespace LAS;

DisplayManager::DisplayManager(const LoggerPtr& setLogger)
    :   logger {setLogger}
{

}
DisplayManager::~DisplayManager(){
    shutdown();
}
bool DisplayManager::init(const std::string& imGuiIniPath){
    if(!setIniPath(imGuiIniPath)){
        logger->log("Could not find/create ImGui INI file at [" + imGuiIniPath + "]", Tags{"ImGui Setup", "ERROR"});
        return false;
    }

    if(!initGLFW())
        return false;

    if(!initImgui(imGuiIniPath))
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

        ImGui::DockSpaceOverViewport();     // Sets up docking for the entire window

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
bool DisplayManager::addWindow(const WindowPtr& window){
    if(!window)
        return false;

    if(windows.contains(window->getID()))
        return false;
    
    for(const auto& w : windows){
        if(w.second->getTitle() == window->getTitle())
            return false;
    }

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
bool DisplayManager::removeWindow(Windowing::Window& window){
    if(!windows.contains(window.getID())){
        return false;
    }
    else{
        windows.erase(window.getID());
        return true;
    }
}
void DisplayManager::closeAllModuleWindows(){
    std::map<uint8_t, WindowPtr>::iterator itr;
    for(itr = windows.begin(); itr != windows.end(); ++itr){
        if(itr->second->getMenuOption() == Windowing::MenuOption::MODULE){
            itr->second->shown = false;
        }
    }
}

void DisplayManager::clearAllModuleWindows(){
    std::map<uint8_t, WindowPtr>::iterator itr;
    for(itr = windows.begin(); itr != windows.end(); ++itr){
        if(itr->second->getMenuOption() == Windowing::MenuOption::MODULE){
            windows.erase(itr);
        }
    }
}

bool DisplayManager::setIniPath (const std::string& path, bool createNewFile){
    if(std::filesystem::exists(path)){
        iniPath = path;
        return true;
    }

    if(!createNewFile)
        return false;
    
    std::ofstream newIniFile {path, std::ios::trunc};

    if(!std::filesystem::exists(path))
        return false;

    // Log that a new file had to be created
    logger->log("ImGui window configuration at \"" + path + "\" was newly created. Prior window configurations were set to default state.", Tags{"WARNING", "Display Manager"});

    iniPath = path;
    return true;
}
std::string DisplayManager::getIniPath() const{
    return iniPath;
}
bool DisplayManager:: saveWindowConfig() const{
    if(iniPath.empty())
        return false;

    ImGui::SaveIniSettingsToDisk(iniPath.c_str());
    return true;
}
void DisplayManager::getAllWindowNames(StringVector& names) const{
    if(windows.empty())
        return;

    if(!names.empty())
        names.erase(names.begin(), names.end());
    
    for(auto& pair : windows)
        names.push_back(pair.second->getTitle());

    return;
}
void DisplayManager::getAllWindowIDs(std::vector<uint8_t>& IDs) const{
    if(windows.empty())
        return;

    if(!IDs.empty())
        IDs.erase(IDs.begin(), IDs.end());
    
    for(auto& pair : windows)
        IDs.push_back(pair.first);

    return;
}
WindowStatus DisplayManager::getWindowStatus(const uint8_t ID) const {
    if(!windows.contains(ID))
        return WindowStatus{0, "NULL", false};

    WindowStatus statusBuf;

    statusBuf.ID    = ID;
    statusBuf.title = windows.at(ID)->getTitle();
    statusBuf.open  = windows.at(ID)->shown;

    return statusBuf;
}
bool DisplayManager::setWindowShownStatus(uint8_t ID, bool shown) const{
    if(!windows.contains(ID))
        return false;
    
    windows.at(ID)->shown = shown;

    return true;
}
uint8_t DisplayManager::getWindowID(const std::string& title) const{
    for(auto& pair : windows){
        if(pair.second->getTitle() == title)
            return pair.first;
    }

    return 0;
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
bool DisplayManager::initImgui(std::string iniFilePath){
    if(iniFilePath.empty())
        iniFilePath = iniPath;

    if(!std::filesystem::exists(iniFilePath)){
        logger->log("Could not find ImGui INI file at [" + iniFilePath + "]", Tags{"ImGui Setup", "ERROR"});
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    io.IniFilename = NULL;                                    // Disables automatic loading and saving of imgui.ini
    ImGui::LoadIniSettingsFromDisk(iniPath.c_str());

    // Setup Platform/Renderer backends

    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    if(!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init() ) {
        logger->log("Could not initialize ImGui with OpenGL/GLFW", Tags{"ImGui Setup", "ERROR"});
        return false;
    }
    
    return true;
    
}
void DisplayManager::drawWindows(){    
    // Draw the menu options
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("Modules")){
            ImGui::MenuItem("(Loaded Modules)", NULL, false, false);
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Utilities")){
            ImGui::MenuItem("(LAS Tools)", NULL, false, false);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    for(auto pair : windows){
        auto w {pair.second};
        
        if(w->shown)
            w->draw();

        if(ImGui::BeginMainMenuBar()){
            if(w->getMenuOption() == Windowing::MenuOption::MODULE){
                if(ImGui::BeginMenu("Modules")){
                    ImGui::MenuItem(w->getTitle(), NULL, &w->shown);
                    ImGui::EndMenu();
                }
            }
            else if(w->getMenuOption() == Windowing::MenuOption::UTILITY){
                if(ImGui::BeginMenu("Utilities")){
                    ImGui::MenuItem(w->getTitle(), NULL, &w->shown);
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMainMenuBar();
        }
    }
}