#pragma once

#include "Ammo.h"
#include "Gun.h"

#include <LAS/Window.h>

namespace ShooterCentral{
    class ShooterCentralWindow : public LAS::Windowing::Window {
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;

        bool setAmmoTracker (AmmoTrackerPtr setAmmoTracker);
        bool setGunTracker  (GunTrackerPtr setGunTracker);


    private:
        static constexpr int NUM_CARTRIDGE_NAMES_SHOWN { 10 };

        void drawStockpile  ()  const;
        void drawArmory     ()  const;

        AmmoTrackerPtr  ammoTracker;
        GunTrackerPtr   gunTracker;

    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}