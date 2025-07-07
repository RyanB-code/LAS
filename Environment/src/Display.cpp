#include "Display.h"

using namespace LAS;
using namespace Display;

bool Display::ensureIniExists (const std::string& path){
    if(std::filesystem::exists(path))
        return true;

    if(LAS::ensureFile(path)){
        log_warn("ImGui window configuration at \"" + path + "\" was newly created. Any prior window configurations were set to default." );
        return true;
    }
    else
        return false;
}

bool Display::initGLFW(GLFWwindow** window, const std::string& title){
    if(!glfwInit()){
        log_critical("Could not initialize GLFW.");
        return false;
    }

    // Hints before createWindow
    glfwWindowHint(GLFW_FOCUSED,    GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED,  GLFW_TRUE);

    
    // Creates the window in windowed mode
    *window = glfwCreateWindow(640, 480, title.c_str(), NULL, NULL);

    if(!window){
        log_critical("Could not obtain window context");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);

    return true;
}
bool Display::initImgui(GLFWwindow** window, const std::string& iniFilePath){

    if(!std::filesystem::exists(iniFilePath)){
        log_warn("Could not find ImGui INI file at [" + iniFilePath + "]");
        return false;
    }


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    io.IniFilename = NULL;                                    // Disables automatic loading and saving of imgui.ini
    ImGui::LoadIniSettingsFromDisk(iniFilePath.c_str());

    // Setup Platform/Renderer backends

    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    if(!ImGui_ImplGlfw_InitForOpenGL(*window, true) || !ImGui_ImplOpenGL3_Init() ) {
        log_critical("Could not initialize ImGui with OpenGL/GLFW");
        return false;
    }
    
    return true;
    
}
std::string Display::makeKey(const std::string& text) {
    std::string key;

    for(const char& c : text)
        key += tolower(c);

    return key;
}



LogWindow::LogWindow() : LogOutput{ } {

}
LogWindow::~LogWindow() {

}
void LogWindow::log(const Log& log) {
    Log logCopy {log.msg, log.tag, log.location, log.timestamp};
    logHistory.push_back(logCopy);
}
void LogWindow::setShown(std::shared_ptr<bool> set) {
    shown = set;
    return;
}
void LogWindow::draw() {
    if(!ImGui::Begin(LOG_WINDOW_NAME, &*shown)){
        ImGui::End();
        return;
    }

    ImGui::BeginChild("Options", ImVec2(ImGui::GetWindowSize().x-20, 85), ImGuiChildFlags_Border);
    static bool autoScroll      { true };
    ImGui::Checkbox("Show Time",            &settings.showTime); 
    ImGui::SameLine();
    ImGui::Checkbox("Show Tags",            &settings.showTags);
    ImGui::SameLine();
    ImGui::Checkbox("Show Message",         &settings.showMsg);
    ImGui::SameLine();
    ImGui::Checkbox("Show Code Location",   &settings.showLocation);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scroll",          &autoScroll);


    ImGui::InputInt("Tag Text Box Size",     &settings.textBoxWidth_tag, 1, 5);
    ImGui::InputInt("Message Text Box Size", &settings.textBoxWidth_msg, 1, 5);
    ImGui::EndChild(); // End Options Child
    
    // Ensure sizes are not too small
    if(settings.textBoxWidth_tag < 5)
        settings.textBoxWidth_tag = 5;
    if(settings.textBoxWidth_msg < 20)
        settings.textBoxWidth_msg = 20;

    // Log window portion
    ImGui::SeparatorText("Logs");
    ImGui::BeginChild("Logs", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

    for(const auto& log : logHistory){
        using namespace LAS::Logging;
        std::ostringstream os{};    // Buffer to store formatted log

        if (settings.showTime)
            os << '[' << printTime(log.timestamp) << "]  ";

        if (settings.showTags) {
            os << '[';
            os << std::format("{:^{}}", log.tag, settings.textBoxWidth_tag);
            os << "] ";       
     
            os << " ";
        }
        if (settings.showMsg){
            if(log.msg.size() > settings.textBoxWidth_msg)
                os << std::format("{:{}}...  ", log.msg.substr(0, settings.textBoxWidth_msg-3), settings.textBoxWidth_msg-3);
            else
                os << std::format("{:<{}}  ", log.msg, settings.textBoxWidth_msg);
        }
        if (settings.showLocation)
            os << printLocation(log.location);

        ImGui::TextUnformatted(os.str().c_str());
    }

    if(autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    

    ImGui::EndChild();
    ImGui::End();
}
DisplayManager::DisplayManager()
{

}
DisplayManager::~DisplayManager(){
    shutdown();
}
bool DisplayManager::init(const std::string& imGuiIniPath){
    using namespace Display;

    if(!ensureIniExists(imGuiIniPath)){
        log_critical("Could not find/create ImGui INI file at [" + imGuiIniPath + "]");
        return false;
    }

    iniPath = imGuiIniPath;

    if(!initGLFW(&window, WINDOW_TITLE))
        return false;

    if(!initImgui(&window, iniPath))
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


        drawWindows();  // MY RENDER CODE HERE


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
std::string DisplayManager::getIniPath() const{
    return iniPath;
}
bool DisplayManager:: saveWindowConfig() const{
    if(iniPath.empty())
        return false;

    ImGui::SaveIniSettingsToDisk(iniPath.c_str());
    return true;
}
bool DisplayManager::addWindow(const std::string& title, std::function<void()> drawFunction){
    std::string key { Display::makeKey(title) };

    if(windowInformation.contains(key))
        return false;

    return windowInformation.try_emplace(key, Info{title, std::make_shared<bool>(false), drawFunction}).second;
}
bool DisplayManager::containsWindow(const std::string& title) const {
    return windowInformation.contains(Display::makeKey(title));
}
std::shared_ptr<bool> DisplayManager::shown(const std::string& title){
    std::string key {Display::makeKey(title)};

    if(windowInformation.contains(key))
        return windowInformation.at(key).shown;
    else
        throw std::runtime_error("Window information with that key does not exist");
}
void DisplayManager::closeAllWindows() const {
    for(auto& [key, value] : windowInformation) {
        *value.shown = false;
    }
}
void DisplayManager::clearModuleWindows() {
    for(auto itr { windowInformation.begin()}; itr != windowInformation.end(); ++itr) {
        auto& info {itr->second};
        if(info.title != LOG_WINDOW_NAME && info.title != SHELL_WINDOW_NAME)
            windowInformation.erase(itr);
    }
}
std::map<std::string, Display::Info>::const_iterator DisplayManager::cbegin() const {
    return windowInformation.cbegin();
}
std::map<std::string, Display::Info>::const_iterator DisplayManager::cend() const {
    return windowInformation.cend();
}
Display::Info& DisplayManager::at(const std::string& title) {
    return windowInformation.at(Display::makeKey(title));
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

    for(auto& [key, value] : windowInformation){
        if(ImGui::BeginMainMenuBar()){
            if(value.title == LOG_WINDOW_NAME || value.title == SHELL_WINDOW_NAME ) {
                if(ImGui::BeginMenu("Utilities")){
                    ImGui::MenuItem(value.title.c_str(), NULL, &*value.shown);
                    ImGui::EndMenu();
                }
            }
            else{
                if(ImGui::BeginMenu("Modules")){
                    ImGui::MenuItem(value.title.c_str(), NULL, &*value.shown);
                    ImGui::EndMenu();
                }
            }   
            
            ImGui::EndMainMenuBar();
        }

        if(*value.shown)
            value.drawFunction();
    }
}




