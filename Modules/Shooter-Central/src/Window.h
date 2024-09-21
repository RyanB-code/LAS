#pragma once

#include "Ammo.h"

#include <LAS/Window.h>

namespace ShooterCentral{
    class ShooterCentralWindow : public LAS::Windowing::Window {
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;

        bool addAmmoTracker(AmmoTrackerPtr setAmmoTracker);

    private:
        static constexpr int NUM_CARTRIDGE_NAMES_SHOWN { 10 };
        void drawStockpile() const;

        AmmoTrackerPtr ammoTracker;

    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}