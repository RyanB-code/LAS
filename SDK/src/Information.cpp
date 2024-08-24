#include <LAS/Information.h>

std::string LAS::Information::versionToStr (const Version& ver){
    std::ostringstream str;

    str << int{ver.major} << "." << int{ver.minor} << "." << int{ver.patch};

    return str.str();
}
