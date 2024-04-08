#include "TextManipulations.h"

namespace TextManipulations{
    std::string ensureSlash(std::string text){
        // Ensure slash
        if(text.back() != '\\' && text.back() != '/'){
            text += "/";
        }
        return text;
    }
}