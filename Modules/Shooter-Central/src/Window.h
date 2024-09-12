#pragma once

#include <LAS/Window.h>

namespace ShooterCentral{
    class ShooterCentralWindow : public LAS::Windowing::Window {
    public:
        ShooterCentralWindow();
        ~ShooterCentralWindow();

        void draw() override;
    private:
        std::string test;
    };

    using SCWindowPtr = std::shared_ptr<ShooterCentralWindow>;
}