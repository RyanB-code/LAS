#pragma once

#include <queue>
#include <string>

#include <imgui/imgui.h>
#include <LAS/Commands.h>
#include <LAS/Window.h>
#include <LAS/Helper.h>

class Console : public LAS::Window{
public:
    Console(std::queue<std::string>& setQueue);
    ~Console();

    void draw() override;

private:
    std::queue<std::string>& queue;
};

