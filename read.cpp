/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:    read.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on January 31, 2018, 8:44 PM
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if(argc==1){
        printf("ATTENTION: no path specified\n"
                "You must specify: read <filename>\n");
    }
//THE LINE BELOW IS TO BE IMPLEMENTED LATER!!!
//    char* pathspec = argv[argc-1] //Stores filename path reference into pointer variable
/* EXCEPTION HANDLING WILL BE NEEDED FOR THE LINE ABOVE
   if(bad condition){throw a function call to "handle" it};
   The function handling it will probably do nothing and will be placed in
   header file read.hpp. This will suppress any difficulties with &&argv
*/
    
    /* TODO_LIST:
     * 
     * Open the file at specified by our char* pathspec
     * Store output into a variable: it will be an integer.
     * Create a class object of our struct class (whatever we name it) and
     *         pass the integer with the call
     * 
     * From there, the class file that contains our struct will do the rest.
     * This file will just run the overhead for all the grunt files doing
     *         the actual work.
     */
    
    
    
    cout << "test" << endl;
    
    return 0;
}

