#pragma once

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
    public:
        TextBuffer();
        ~TextBuffer() = default;

        void push(const std::string& text);
        void clear();

        void writeToScreen() const;

        int getMaxCharPerLine() const { return MAX_CHAR_LINE - 1;  }
    private:
        static constexpr int MAX_CHAR_LINE  { 256 };
        static constexpr int MAX_LINES      { 64 };

        char textBuffer [MAX_LINES][MAX_CHAR_LINE];
        
        char* memBlockStart { nullptr };
        char* memBlockEnd   { nullptr };

        char* firstLine { nullptr };
        char* nextLine  { nullptr };

        void incLine(char** line) const;
    };
}   // End LAS

