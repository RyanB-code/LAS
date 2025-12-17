#pragma once

#include <functional>
#include <string>

namespace LAS {

    struct TaggedUpdateFunction {
        const std::string tag;
        const std::function<void()> function;
    };
    struct TaggedDrawFunction {
        const std::string tag;
        const std::function<void()> function;
        bool shown { false };
    };
}   // End LAS

