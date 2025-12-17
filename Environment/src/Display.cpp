#include "Display.h"

using namespace LAS;
using namespace LAS::Logging;

namespace LAS::Display {

bool ensureIniExists (const std::string& path){
    if(std::filesystem::exists(path))
        return true;

    if(LAS::ensureFile(path)){
        log_warn("ImGui window configuration at \"" + path + "\" was newly created. Any prior window configurations were set to default." );
        return true;
    }
    else
        return false;
}

bool initGLFW(GLFWwindow** window, const std::string& title){
    if(!glfwInit()){
        log_fatal("Could not initialize GLFW.");
        return false;
    }

    // Hints before createWindow
    glfwWindowHint(GLFW_FOCUSED,    GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED,  GLFW_TRUE);

    
    // Creates the window in windowed mode
    *window = glfwCreateWindow(640, 480, title.c_str(), NULL, NULL);

    if(!window){
        log_fatal("Could not obtain window context");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);

    return true;
}
bool initImgui(GLFWwindow** window, const std::string& iniFilePath){

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
        log_fatal("Could not initialize ImGui with OpenGL/GLFW");
        return false;
    }
    
    return true;
    
}
std::string makeKey(const std::string& text) {
    std::string key;

    for(const char& c : text)
        key += tolower(c);

    return key;
}



LogWindow::LogWindow() : LogOutput{ } {

}
void LogWindow::log(const Log& log) {
    Log logCopy {log.msg, log.severityTag, log.moduleTag, log.location, log.timestamp};
    logHistory.push_back(logCopy);
}
void LogWindow::draw() {
    static bool autoScroll      { true };

    if(ImGui::BeginChild("Options", ImVec2(ImGui::GetWindowSize().x-20, 85), ImGuiChildFlags_Border)){
        ImGui::Checkbox("Show Time",            &settings.showTime); 
        ImGui::SameLine();
        ImGui::Checkbox("Show Severity Tag",    &settings.showSeverityTag);
        ImGui::SameLine();
        ImGui::Checkbox("Show Module Tag",      &settings.showModuleTag);
        ImGui::SameLine();
        ImGui::Checkbox("Show Message",         &settings.showMsg);
        ImGui::SameLine();
        ImGui::Checkbox("Show Code Location",   &settings.showLocation);
        ImGui::SameLine();
        ImGui::Checkbox("Auto Scroll",          &autoScroll);
        ImGui::InputInt("Tag Text Box Size",     &settings.textBoxWidth_tag, 1, 5);
        ImGui::InputInt("Message Text Box Size", &settings.textBoxWidth_msg, 1, 5);
    }
    ImGui::EndChild(); // End Options Child
    
    // Ensure sizes are not too small
    if(settings.textBoxWidth_tag < 5)
        settings.textBoxWidth_tag = 5;
    if(settings.textBoxWidth_msg < 20)
        settings.textBoxWidth_msg = 20;

    // Log window portion
    ImGui::SeparatorText("Logs");
    if(ImGui::BeginChild("Logs", ImVec2(0,0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)){

        for(const auto& log : logHistory){
            using namespace LAS::Logging;
            std::ostringstream os{};    // Buffer to store formatted log

            if (settings.showTime)
                os << std::format("[{}]  ", printTime(log.timestamp));

            if(settings.showSeverityTag)
                os << std::format("[{:^{}}]  ", log.severityTag, settings.textBoxWidth_tag);

            if(settings.showModuleTag)
                os << std::format("[{:^{}}]  ", log.moduleTag, settings.textBoxWidth_tag);  

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
    } 
    ImGui::EndChild();
}


}   // End LAS::Display namespace









DisplayManager::~DisplayManager(){
    shutdown();
}
bool DisplayManager::init(const std::string& imGuiIniPath){
    using namespace Display;

    if(!ensureIniExists(imGuiIniPath)){
        log_fatal("Could not find/create ImGui INI file at [" + imGuiIniPath + "]");
        return false;
    }

    iniPath = imGuiIniPath;

    if(!initGLFW(&window, WINDOW_TITLE))
        return false;

    if(!initImgui(&window, iniPath))
        return false;

    return true;    
}
bool DisplayManager::refresh(std::map<std::string, TaggedDrawFunction>& list){

    if(!glfwWindowShouldClose(window)){
        glfwPollEvents();                   // Poll for and process events

        // Starts the Imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport();     // Sets up docking for the entire window


        drawWindows(list);  // MY RENDER CODE HERE


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
bool DisplayManager:: saveWindowConfig() const{
    if(iniPath.empty())
        return false;

    ImGui::SaveIniSettingsToDisk(iniPath.c_str());
    return true;
}
bool DisplayManager::addInternalWindow(
        const std::string& title,
        const std::string& tag, 
        std::function<void()> draw 
    )
{
    std::string key { Display::makeKey(title) };

    if(internalWindows.contains(key))
        return false;

    return internalWindows.try_emplace(title, TaggedDrawFunction{tag, draw} ).second;
}
void DisplayManager::drawWindows(std::map<std::string, TaggedDrawFunction>& list){    
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

    for(auto& [title, items] : list){
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Modules")){
                ImGui::MenuItem(title.c_str(), NULL, &items.shown);
                ImGui::EndMenu();
            }   
            
            ImGui::EndMainMenuBar();
        }

        Logging::setModuleTag(items.tag);
        if(items.shown){
            // ---------------------
            // The commented out code here if for timing how long each Module takes to render
            // -------------------
            //using namespace std::chrono;
            //auto before { steady_clock::now() };

            if(ImGui::Begin(title.c_str(), &items.shown)){
                items.function();
            }
            ImGui::End();

            //auto after { steady_clock::now() };
            //std::cout << std::format("{} draw: {}\n", key, duration_cast<milliseconds>(after - before).count());
        }
        Logging::setModuleTag("LAS");
    }



    for(auto& [title, items] : internalWindows){
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Utilities")){
                ImGui::MenuItem(title.c_str(), NULL, &items.shown);
                ImGui::EndMenu();
            }   
            
            ImGui::EndMainMenuBar();
        }

        Logging::setModuleTag(items.tag);
        if(items.shown){
            // ---------------------
            // The commented out code here if for timing how long each Module takes to render
            // -------------------
            //using namespace std::chrono;
            //auto before { steady_clock::now() };
            if(ImGui::Begin(title.c_str(), &items.shown)){
                items.function();
            }
            ImGui::End();
            //auto after { steady_clock::now() };
            //std::cout << std::format("{} draw: {}\n", key, duration_cast<milliseconds>(after - before).count());
        }
        Logging::setModuleTag("LAS");
    }}




