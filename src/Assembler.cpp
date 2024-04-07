#include <string>

int main(){

    #ifdef __linux__
        std::string directory {"/mnt/NAS/1-Project-Related/Project-Source-Directories/LAS-Test/"};
		std::string settingsPath {directory + ".las-rc"};
		//std::string home {getenv("HOME")};
		//std::string settingsPath { home + "/.las-rc"};
		//std::string directory {home + "/Documents/LAS/"};
    #endif
    #ifdef _Win32
        std::cout << "Windows is not currently supported.\nApplication aborted.\n";
        return -1;
    #endif

    

    return 0;
}