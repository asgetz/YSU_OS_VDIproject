/* 
 * File:    read.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on January 31, 2018, 8:44 PM
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "ext2_fs.h"
#include "read.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

//#include <cstdlib>
//#include <iostream>
//#include <stdio.h>
//#include <fcntl.h>

#include "Virtual.cpp"
#include "ext2.cpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc==1){
        printf("ATTENTION: no path specified\n"
                "You must specify: read <filename>\n");
        return 0;
    }
    
    char* path = argv [argc - 1];
    int file = open(path, O_RDONLY);
    
    if (file == -1) {
        throw fileUnreadable();
    }
    
    VBox vbox (file);
    
    
    
    ext2 ext2FileSystem (&vbox);
//    ext2FileSystem.verifySuper();
//    ext2FileSystem.bGroupTableDump();
//    ext2FileSystem.verifyNodes();
    
    std::cout << std::endl;
    return 0;
}