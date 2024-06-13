#pragma once


#include <LAS/Window.h>
#include <LAS/Logging.h>

#include <imgui/imgui.h>
#include <iostream>

struct ModuleInfo    {
    std::string     title           {};
    std::string     shortTitle      {};
    WindowPtr       window          {};
};
struct EnvironmentInfo     {
    std::string     directory{};
    
    ImGuiContext&   context;
    LoggerPtr       logger;
};

#if __GNUC__ >= 4
    #define MODULE_EXPORT extern "C" __attribute__ ((visibility ("protected")))
#endif

MODULE_EXPORT bool      LASM_load       (EnvironmentInfo, ModuleInfo&);
MODULE_EXPORT void      LASM_cleanup    ();

