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
        AmmoTrackerPtr ammoTracker;


    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}