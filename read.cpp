/*
 * File:    read.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on January 31, 2018, 8:44 PM
 */

// Headers to include
#include <fcntl.h>
#include <iostream>
#include <string>

// Additional files to include
#include "read.h"
#include "Virtual.cpp"
#include "ext2.cpp"

using namespace std;

int main(int argc, char** argv) {
    // Check to make sure that the file name is valid and tell the user if it isn't
    if (argc==1){
        printf("ERROR: You did not specify a valid file name.\n");
        printf("A valid file name has the format: read <fileName>\n");
        printf("If you have entered the file name in the correct format, you may be attempting to open a non-valid file type.\n");
        printf("Please make sure that that is not the case before trying again.\n");
        return 0;
    }
    char* path = argv [argc - 1]; // Store the path to where the file is located
    int readFile = open(path, O_RDWR); // Store the file specified where the path goes
    if (readFile < 0) {throw fileUnreadable();} // If the file is not a valid file, throw the respective error

    VBox Box (readFile); // Construct a VBox file using the file to read

    ext2 ext2Reader (&Box); // Construct the ext2 file structure that we'll be using

    ext2Reader.verifySuper();
    printf("Super block integrity: CHECKED\n\n");

    ext2Reader.verify_bGroupTable();
    printf("Block Group Descriptor Table integrity: CHECKED\n\n");

    ext2Reader.verifyNodes();
    printf("I-Node integrity: CHECKED\n\n");



    printf("Done.\n"); // Print out to confirm end of the program
    return 0;

}
