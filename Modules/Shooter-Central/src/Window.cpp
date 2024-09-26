#include "Window.h"

using namespace ShooterCentral;


ShooterCentralWindow::ShooterCentralWindow() : LAS::Windowing::Window{"Shooter Central"} 
{

}
ShooterCentralWindow::~ShooterCentralWindow(){

}
void ShooterCentralWindow::draw() {
    if(ImGui::Begin(title.c_str(), &shown)){

        if(!ammoTracker || !gunTracker){
            ImGui::Text("This module has not been setup yet...");
            ImGui::End();
            return;
        }

        drawArmory();
        drawStockpile();

    }
    ImGui::End();
}
bool ShooterCentralWindow::setAmmoTracker(AmmoTrackerPtr setAmmoTracker){
    if(!setAmmoTracker)
        return false;

    ammoTracker = setAmmoTracker;
    return true;
}
bool ShooterCentralWindow::setGunTracker(GunTrackerPtr setGunTracker){
    if(!setGunTracker)
        return false;

    gunTracker = setGunTracker;
    return true;
}

// MARK: PRIVATE FUNCTIONS

// MARK: STOCKPILE
void ShooterCentralWindow::drawStockpile() const{
    if(ImGui::BeginChild("Stockpile", ImVec2{500, 250})){

        static StringVector ammoNames;
        static StringVector cartridgeNames;
        static std::vector<std::pair<std::string, uint64_t>> countByCartridge;
        static std::vector<TrackedAmmo>    ammo;

        ammoTracker->getAllAmmoNames(ammoNames);
        ammoTracker->getAllCartridgeNames(cartridgeNames);
        ammoTracker->getAmmoCountByCartridge(countByCartridge);
        ammoTracker->getAllAmmo(ammo);

        ImGui::Text("Stockpile");

        static bool detailedView { false };
        ImGui::Checkbox("Detailed View", &detailedView);

        if(detailedView){
            static int  highlilghtedItem        { 0 };
            static bool showAllAmmo             { false };
            static int  columnsInDetailedTable  { 5 };

            const char* cartridgeList[NUM_CARTRIDGE_NAMES_SHOWN];
            cartridgeList[0] = "ALL";       // Default option

            static std::vector<TrackedAmmo>    selectedCatridgeAmmoList;
            int i { 1 };                                    // Starts at one to offset the all option
            for(const auto& s : cartridgeNames){
                if(i < (NUM_CARTRIDGE_NAMES_SHOWN+1)){      // Add one to offset the additional 'all' option
                    cartridgeList[i] = s.c_str();
                    ++i;
                }
            }

            ImGui::Combo("Cartridge", &highlilghtedItem, cartridgeList, cartridgeNames.size()+1); // This is the selector table. Add one to account for 'all' option
            ammoTracker->getAllAmmoByCartridge(selectedCatridgeAmmoList, cartridgeList[highlilghtedItem]);  // Populates list of ammo with selected cartridge

            // Change table based on what is highlighted
            if(highlilghtedItem == 0){
                showAllAmmo = true;
                columnsInDetailedTable = 5;
            }
            else{
                showAllAmmo = false;
                columnsInDetailedTable = 4;
            }

            if(ImGui::BeginTable("Detailed Cartridge Breakdown", columnsInDetailedTable, 
                                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                                    ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                                    ImGuiTableFlags_NoHostExtendX
                                ))
            {
                if(showAllAmmo)
                    ImGui::TableSetupColumn("Cartridge",    0, 100);
                ImGui::TableSetupColumn("Name",         0, 100);
                ImGui::TableSetupColumn("Manufacturer", 0, 100);
                ImGui::TableSetupColumn("Grain Weight", 0, 100);
                ImGui::TableSetupColumn("Amount",       0, 100);
                ImGui::TableHeadersRow();

                if(showAllAmmo){
                    for(auto itr { ammo.begin() }; itr != ammo.end(); ++itr){
                        ImGui::TableNextRow();
                        for (int column{0}; column < columnsInDetailedTable; ++column)
                        {
                            ImGui::TableSetColumnIndex(column);
                            std::ostringstream text;
                            switch( column ){
                                case 0:
                                    ImGui::Text(itr->ammoType.cartridge.c_str());
                                    break;
                                case 1:
                                    ImGui::Text(itr->ammoType.name.c_str());
                                    break;
                                case 2:
                                    ImGui::Text(itr->ammoType.manufacturer.c_str());
                                    break;
                                case 3:
                                    text << int{itr->ammoType.grainWeight};
                                    ImGui::Text(text.str().c_str());
                                    break;
                                case 4:
                                    text << itr->amount;
                                    ImGui::Text(text.str().c_str());
                                    break;
                                default:
                                    ImGui::Text("Broken table");
                                    break;
                            }
                        }        
                    } // End populating all table
                }
                else{
                    for(auto itr { selectedCatridgeAmmoList.begin() }; itr != selectedCatridgeAmmoList.end(); ++itr){
                        ImGui::TableNextRow();
                        for (int column{0}; column < columnsInDetailedTable; ++column)
                        {
                            ImGui::TableSetColumnIndex(column);
                            switch( column ){
                                case 0:
                                    ImGui::Text(itr->ammoType.name.c_str());
                                    break;
                                case 1:
                                    ImGui::Text(itr->ammoType.manufacturer.c_str());
                                    break;
                                case 2:
                                    ImGui::Text("%d", int{itr->ammoType.grainWeight});
                                    break;
                                case 3:
                                    ImGui::Text("%lu", itr->amount);
                                    break;
                                default:
                                    ImGui::Text("Broken table");
                                    break;
                            }
                        }        
                    } // End populating selected cartridge table
                }

                ImGui::EndTable();
            } // End table

            ImGui::Separator();

        } // End detailed view


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
                        switch( column ){
                            case 0:
                                ImGui::Text(itr->first.c_str());
                                break;
                            case 1:
                                ImGui::Text("%lu", itr->second);
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }
                }

                ImGui::EndTable();
            } // End table
        }
    }
    ImGui::EndChild();
}
// MARK: ARMORY
void ShooterCentralWindow::drawArmory() const{
    if(ImGui::BeginChild("Armory", ImVec2{500, 250})){       
        static std::vector<Gun> gunList;

        gunTracker->getAllGuns(gunList);

        ImGui::Text("Armory");

        static int gunNum { 0 };
        if(ImGui::Button("Add gun")){
            std::ostringstream name;
            name << "Gun " << gunNum;
            AmmoType ammoType1 {"Test name", "Test man", "Test Cart", 69};
            AmmoType ammoType2 {"Test name2", "Test man", "Test Cart", 50};

            GunPtr gunBuf { gunTracker->createPistol(name.str(), "test cartridge")};

            if(gunBuf){
                gunBuf->addToRoundCount(15, ammoType1);
                gunBuf->addToRoundCount(20, ammoType2);

                ++gunNum;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Write guns")){
            gunTracker->writeAllGuns();
        }
        ImGui::SameLine();
        if(ImGui::Button("Read guns")){
            gunTracker->readGuns();
        }

        ImGui::Text("Guns tracked %lu", gunTracker->getGunTotal());

        if(ImGui::BeginTable("Gun Table", 4, 
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                    ImGuiTableRowFlags_Headers | ImGuiTableFlags_Resizable |
                                    ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_HighlightHoveredColumn |
                                    ImGuiTableFlags_NoHostExtendX
                            ))
        {
            ImGui::TableSetupColumn("Weapon Type",  NULL, 100);
            ImGui::TableSetupColumn("Cartridge",    NULL, 100);
            ImGui::TableSetupColumn("Name",         NULL, 100);
            ImGui::TableSetupColumn("Round Count",  NULL, 100);

            ImGui::TableHeadersRow();


            for(auto itr { gunList.begin() }; itr != gunList.end(); ++itr){
                    ImGui::TableNextRow();
                    for (int column{0}; column < 4; ++column)
                    {
                        const Gun& gun {*itr};

                        ImGui::TableSetColumnIndex(column);
                        switch( column ){
                            case 0:
                                ImGui::Text(GunHelper::weaponTypeToStr(gun.getWeaponType()).c_str());
                                break;
                            case 1:
                                ImGui::Text(gun.getCartridge().c_str());
                                break;
                            case 2:
                                ImGui::Text(gun.getName().c_str());
                                break;
                            case 3:
                                ImGui::Text("%lu", gun.getRoundCount());
                                break;
                            default:
                                ImGui::Text("Broken table");
                                break;
                        }
                    }        
                }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}