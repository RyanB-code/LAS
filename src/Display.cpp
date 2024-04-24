#include "Display.h"

DisplayManager::DisplayManager(const Logger& setLogger)
    : logger {setLogger}
{

}
DisplayManager::~DisplayManager(){

}
bool DisplayManager::init(){
    return false;
}
bool DisplayManager::refresh(){
    return false;

}
bool DisplayManager::initGLFW(){
    if(!glfwInit()){
        
    }
    return false;

}
bool DisplayManager::initImgui(){
    return false;
    
}
bool DisplayManager::initOpenGL(){
    return false;
    
}