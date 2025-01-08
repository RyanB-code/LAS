#include <LAS/HelperFunctions.h>



// MARK: LAS Namespace
namespace LAS{

    bool ensureDirectory (const std::string& path){
        if(std::filesystem::exists(path)){
            return true;
        }
        else{
            return std::filesystem::create_directories(path);
        }
    }
    bool ensureFile     (const std::string& path){
        if(std::filesystem::exists(path)){
            return true;
        }
        else{
            std::ofstream file(path);
            return std::filesystem::exists(path);
        }
    }
}

namespace LAS::TextManip{
    std::string ensureSlash(std::string text){
        // Ensure slash
        if(text.back() != '\\' && text.back() != '/'){
            text += "/";
        }
        return text;
    }
    std::string ensureNoSpaces  (const std::string& text){
        std::string textBuf;
        for(const char& c : text){
            if(c == ' ')
                textBuf += '-';
        }
        return textBuf;
    }
    std::string ensureAlNumNoSpaces (const std::string& text){
        std::string textBuf;
        for(const char& c : text){
            if(std::isalnum(c))
                textBuf += c;
            if(c == ' ')
                textBuf += '-';
        }
        return textBuf;
    }
    std::string ensureNewline (std::string text ){
        if(text.empty())
            return std::string{};

        // Ensure newline
        if(text.back() != '\n'){
            text += '\n';
        }
        return text;
    }

}
