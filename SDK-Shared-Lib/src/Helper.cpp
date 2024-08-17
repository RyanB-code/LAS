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

}

namespace LAS::TextManip::Logging{

    std::string printTime(const std::chrono::system_clock::time_point& time) noexcept {
        std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time }; // Makes zoned time

        // Needed to output the seconds as 2 digits instead of a float
        std::string seconds {std::format("{:%S}", zonedTime)};
        std::ostringstream formattedTime;
        formattedTime << std::format("{:%H:%M:}", zonedTime) << seconds.substr(0,2);

        return formattedTime.str();
    }
    std::string printLocation(const std::source_location& location)	noexcept {
        std::ostringstream os;

        os << "[" << location.file_name() << " | " << location.function_name() << " | " << location.line() << "]";

        return os.str();
    }
}