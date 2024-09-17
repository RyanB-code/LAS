#include "Window.h"

using namespace ShooterCentral;


ShooterCentralWindow::ShooterCentralWindow() : LAS::Windowing::Window{"Shooter Central"} 
{

}
ShooterCentralWindow::~ShooterCentralWindow(){

}
void ShooterCentralWindow::draw() {
    if(ImGui::Begin(title.c_str(), &shown)){
        ImGui::Text("Working");

        static StringVector ammoNames;
        static StringVector cartridgeNames;
        ammoTracker->getAllAmmoNames(ammoNames);
        ammoTracker->getAllCartridgeNames(cartridgeNames);

        ImGui::Text("Ammo: ");
        for(const auto& s : ammoNames){
            ImGui::BulletText(s.c_str());
        }
        ImGui::Text("Cartridge: ");
        for(const auto& s : cartridgeNames){
            ImGui::BulletText(s.c_str());
        }
    }
    ImGui::End();
}
bool ShooterCentralWindow::addAmmoTracker(AmmoTrackerPtr setAmmoTracker){
    if(!setAmmoTracker)
        return false;

    ammoTracker = setAmmoTracker;
    return true;
}