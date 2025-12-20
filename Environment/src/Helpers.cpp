#include "Helpers.h"

namespace LAS {

TextBuffer::TextBuffer() {
    firstLine = memBlockFirstLine;
    nextLine = firstLine;

    memset(textBuffer, 0, sizeof(textBuffer));
}
void TextBuffer::push(const std::string& input) {
    static const char* delimiters = "\n";

    size_t inputLength { input.size() + 1 };

    char* inputBuffer = new char[inputLength];                  // De-allocate at end

    std::strncpy(inputBuffer, input.c_str(), inputLength);
    inputBuffer[inputLength - 1] = '\0';

    char* token = std::strtok(inputBuffer, delimiters);
    do{
        std::strncpy(nextLine, token, MAX_CHAR_LINE - 1);
        nextLine[MAX_CHAR_LINE - 1] = '\0';

        incrementLine(&nextLine);

        if(nextLine == firstLine){
            incrementLine(&firstLine);
        }
        
        token = std::strtok(nullptr, delimiters); 
    }
    while(token);
    delete[] inputBuffer;                                       // De-allocation of inputBuffer

    // Display the buffer
    /*
    for(int i { 0 }; i < MAX_LINES; ++i){
        std::cout << i << ":\t" << textBuffer[i] << '\n';
        
        if(textBuffer[i][0] == '\0')
            std::cout << '\n';
    }
    */
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
            ImGui::Text("%s", lineToPrint);

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
