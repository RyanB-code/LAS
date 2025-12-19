#include "Helpers.h"

namespace LAS {

TextBuffer::TextBuffer() {
    memBlockStart = &textBuffer[0][0];
    memBlockEnd = &textBuffer[MAX_LINES -1 ][MAX_CHAR_LINE];

    firstLine = memBlockStart;
    nextLine = firstLine;

    memset(textBuffer, 0, sizeof(textBuffer));
}
void TextBuffer::push(const std::string& text) {
    int lineSize { sizeof(textBuffer[0]) };

    // Copy up to total allowed size
    std::strncpy(nextLine, text.c_str(), lineSize - 1);
    nextLine[lineSize - 1] = '\0';

    incLine(&nextLine);

    if(nextLine == firstLine){
        incLine(&firstLine);
    }
}

void TextBuffer::clear(){
    memset(textBuffer, 0, sizeof(textBuffer));
    firstLine = memBlockStart;
    nextLine = firstLine;
}
void TextBuffer::writeToScreen() const {

    char* lineToPrint   { firstLine };
    char* nextPrintLine { lineToPrint };

    incLine(&nextPrintLine);

    while(nextPrintLine != firstLine) {

        if(*lineToPrint != 0)
            ImGui::Text(lineToPrint);

        incLine(&lineToPrint);
        incLine(&nextPrintLine);
    }
}
void TextBuffer::incLine(char** line) const{
    int lineSize { sizeof(textBuffer[0]) };

    *line += lineSize;

    if(*line >= memBlockEnd)
       *line = memBlockStart;
}

}   // End LAS
