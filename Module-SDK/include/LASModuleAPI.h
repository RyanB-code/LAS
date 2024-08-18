#pragma once


#include <LAS/Window.h>
#include <LAS/Logging.h>
#include <LAS/Commands.h>
#include <LAS/Information.h>
#include <LAS/SDK.h>


#include <imgui/imgui.h>
#include <iostream>

#if __GNUC__ >= 4
    #define MODULE_EXPORT extern "C" __attribute__ ((visibility ("protected")))
#endif

MODULE_EXPORT bool      LASM_loadModuleInfo         (LAS::Information::ModuleInfo&);            // Overwrite the parameter with your module's information
MODULE_EXPORT bool      LASM_loadEnvironmentInfo    (const LAS::Information::EnvironmentInfo&); // This is information given by the environment to you to do what you want
MODULE_EXPORT void      LASM_cleanup                ();                                         // Called upon closing of your module

