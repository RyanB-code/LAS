#pragma once

#include <LAS/HelperFunctions.h>

#include <functional>
#include <string>
#include <imgui/imgui.h>
#include <cstring>

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

    class TextBuffer {
    private:
        static constexpr int MAX_CHAR_LINE  { 256 };
        static constexpr int MAX_LINES      { 64 };
    public:
        TextBuffer();
        ~TextBuffer() = default;

        void push(std::string);
        void clear();

        void writeToScreen() const;

        int getMaxCharPerLine() const { return MAX_CHAR_LINE - 1;  }
    private:
        char textBuffer [MAX_LINES][MAX_CHAR_LINE];
        
        char* memBlockFirstLine { &textBuffer[0][0] };
        char* memBlockLastLine  { &textBuffer[MAX_LINES -1 ][MAX_CHAR_LINE] };

        char* firstLine { nullptr };
        char* nextLine  { nullptr };

        void incrementLine(char** line) const;
    };
}   // End LAS

