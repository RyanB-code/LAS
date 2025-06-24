#include "Framework.h"

#include <string>
#include <iostream>


int main(int argc, char **argv){
    LAS::Framework framework { };
    
    if(framework.setup()){
        framework.run();
    }
    else{
        std::cerr << "Application could not be setup.\n";
        return -1;
    }

    return 0;
} 

