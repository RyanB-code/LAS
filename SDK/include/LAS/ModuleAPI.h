#pragma once

#include "Window.h"
#include "Logging.h"
#include "Commands.h"
#include "Information.h"
#include "SDK.h"

#if __GNUC__ >= 4
    #define MODULE_EXPORT extern "C" __attribute__ ((visibility ("protected")))
#endif

MODULE_EXPORT bool      LASM_loadModuleInfo         (LAS::Information::ModuleInfo&);            // Overwrite the parameter with your module's information
MODULE_EXPORT bool      LASM_init                   (const LAS::Information::EnvironmentInfo&); // This is information given by the environment to you to do what you want
MODULE_EXPORT void      LASM_cleanup                ();                                         // Called upon closing of your module

