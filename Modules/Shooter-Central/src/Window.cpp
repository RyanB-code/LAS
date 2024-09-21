#include "Window.h"

using namespace ShooterCentral;


ShooterCentralWindow::ShooterCentralWindow() : LAS::Windowing::Window{"Shooter Central"} 
{

}
ShooterCentralWindow::~ShooterCentralWindow(){

}
void ShooterCentralWindow::draw() {
    if(ImGui::Begin(title.c_str(), &shown)){

        if(!ammoTracker){
            ImGui::Text("This module has not been setup yet...");
            ImGui::End();
            return;
        }

        drawStockpile();

    }
    ImGui::End();
}
bool ShooterCentralWindow::addAmmoTracker(AmmoTrackerPtr setAmmoTracker){
    if(!setAmmoTracker)
        return false;

    ammoTracker = setAmmoTracker;
    return true;
}

// MARK: PRIVATE FUNCTIONS
void ShooterCentralWindow::drawStockpile() const{
    if(ImGui::BeginChild("Stockpile")){

        static StringVector ammoNames;
        static StringVector cartridgeNames;
        static std::vector<std::pair<std::string, uint64_t>> countByCartridge;

        ammoTracker->getAllAmmoNames(ammoNames);
        ammoTracker->getAllCartridgeNames(cartridgeNames);
        ammoTracker->getAmmoCountByCartridge(countByCartridge);

        static bool detailedView { false };

        ImGui::Checkbox("Detailed View", &detailedView);

        if(detailedView){
            static int highlilghtedItem;
            const char* items[64];

            int i { 0 };
            for(const auto& s : cartridgeNames){
                items[i] = s.c_str();
                ++i;
            }
            ImGui::Combo("Cartridge", &highlilghtedItem, items, cartridgeNames.size());

            // Show highlighted item detailed table

        }

        if(cartridgeNames.empty())
            ImGui::Text("There are no tracked cartridges.");
        else{
            if(ImGui::BeginTable("Stockpile", 2, 
                                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                    ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                                    ImGuiTableFlags_NoHostExtendX
                                                    ))
            {
                ImGui::TableSetupColumn("Cartridge",    NULL, 100);
                ImGui::TableSetupColumn("Amount",       NULL, 100);
                ImGui::TableHeadersRow();

                // Display each item
                for(auto itr { countByCartridge.begin() }; itr != countByCartridge.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 2; ++column)
                    {
                        ImGui::TableSetColumnIndex(column);
                        if(column == 0)
                            ImGui::Text(itr->first.c_str());
                        else{
                            std::ostringstream text;
                            text << itr->second;
                            ImGui::Text(text.str().c_str());
                        }
                    }        
                }

                ImGui::EndTable();
            } // End table
        }
               

        ImGui::EndChild();
    }
}