#pragma once

#include "Framework.h"

#include <LAS/Window.h>

namespace ShooterCentral{
    class ShooterCentralWindow : LAS::Windowing::Window{
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;
    private:
        
    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}