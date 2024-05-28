#include "Framework.h"

#include <string>
#include <iostream>


int main(int argc, char **argv){
    // For handling commands
    // std::vector<std::string>args {argv, argv + argc};

    Framework framework { };
    if(framework.setup()){
        framework.run();
    }

    return 0;
}