#include "Framework.h"

#include <string>
#include <iostream>


int main(int argc, char **argv){

    std::vector<std::string>args {argv, argv + argc};

    Framework framework { };
    if(framework.setup()){
        framework.run();
    }


    std::cout << "end\n";
    return 0;
}