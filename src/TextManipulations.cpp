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

namespace TextManipulations::Logging{

    std::string printTime(const std::chrono::system_clock::time_point& time) noexcept {
        return std::format("{:%OH:%OM:%OS}", time);
    }
    std::string printLocation(const std::source_location& location)	noexcept {
        std::ostringstream os;

        os << "[" << location.file_name() << " | " << location.function_name() << " | " << location.line() << "]";

        return os.str();
    }
}