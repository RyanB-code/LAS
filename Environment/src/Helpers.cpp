#include "Helpers.h"

namespace LAS {

TextBuffer::TextBuffer() {
    firstLine = memBlockFirstLine;
    nextLine = firstLine;

    memset(textBuffer, 0, sizeof(textBuffer));

    std::cout << "First Line: " << static_cast<void*>(memBlockFirstLine) << "\n";
    std::cout << "[0][0]:     " << static_cast<void*>(&textBuffer[0][0]) << "\n";

}
void TextBuffer::push(const std::string& text) {
    int lineSize { sizeof(textBuffer[0]) };

    // Copy up to total allowed size
    std::strncpy(nextLine, text.c_str(), lineSize - 1);
    nextLine[lineSize - 1] = '\0';

    incrementLine(&nextLine);

    if(nextLine == firstLine){
        incrementLine(&firstLine);
    }
}

void TextBuffer::clear(){
    memset(textBuffer, 0, sizeof(textBuffer));
    firstLine = memBlockFirstLine;
    nextLine = firstLine;
}
void TextBuffer::writeToScreen() const {

    char* lineToPrint   { firstLine };
    char* nextPrintLine { lineToPrint };

    incrementLine(&nextPrintLine);

    while(nextPrintLine != firstLine) {

        if(*lineToPrint != 0)
            ImGui::Text(lineToPrint);

        incrementLine(&lineToPrint);
        incrementLine(&nextPrintLine);
    }
}
void TextBuffer::incrementLine(char** line) const{
    int lineSize { sizeof(textBuffer[0]) };

    *line += lineSize;

    if(*line >= memBlockLastLine)
       *line = memBlockFirstLine;
}

}   // End LAS
