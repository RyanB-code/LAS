#pragma once
#include <string>
#include <chrono>
#include <source_location>
#include <filesystem>
#include <fstream>


namespace LAS{
    bool ensureDirectory    (const std::string& path);
    bool ensureFile         (const std::string& path);

    namespace TextManip {
        std::string ensureSlash(std::string text);

        namespace Logging{
            std::string printTime(const std::chrono::system_clock::time_point& time)    noexcept;   // HH:MM:SS
            std::string printLocation(const std::source_location& location)             noexcept;   // <File Name> | <Function Name> | <Line> 
        }
    }

}