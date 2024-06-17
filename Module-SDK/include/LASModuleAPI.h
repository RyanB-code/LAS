#pragma once


#include <LAS/Window.h>
#include <LAS/Logging.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>

#include <imgui/imgui.h>
#include <iostream>

#if __GNUC__ >= 4
    #define MODULE_EXPORT extern "C" __attribute__ ((visibility ("protected")))
#endif

MODULE_EXPORT bool      LASM_load       (EnvironmentInfo, ModuleInfo&);
MODULE_EXPORT void      LASM_cleanup    ();

