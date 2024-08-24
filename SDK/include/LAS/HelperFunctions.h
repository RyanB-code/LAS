#pragma once
#include <string>
#include <chrono>
#include <source_location>
#include <filesystem>
#include <fstream>

using StringVector = std::vector<std::string>;

namespace LAS {
    bool ensureDirectory    (const std::string& path);
    bool ensureFile         (const std::string& path);

    namespace TextManip {
        std::string ensureSlash         (std::string text);
        std::string ensureNoSpaces      (const std::string& text);
        std::string ensureAlNumNoSpaces (const std::string& text);
    }
}