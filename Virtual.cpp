/* 
 * File:    Virtual.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on January 31, 2018, 8:44 PM
 */

#ifndef VIRTUAL
#define VIRTUAL

// Headers to include
#include <cstring>
#include <unistd.h>

// Files to include
#include "Virtual.hpp"

// Virtual Box constructor, passed in a filename
VBox::VBox(int filename) {
    
    name = filename; // Set the name private variable to filename
    
    _lseek(name, 0, SEEK_SET); // Get the offset from the beginning of the file
    
    read(name, &head, sizeof(head)); // Take the bytes from the file and store them in head
    
    _lseek(name, head.offset_blocks, SEEK_SET); // Get the offset from the file starting at the last part of head 
    
    iMap = new __s32[head.blocks_in_hdd]; // Prep an array the size of the data in head
    read(name, iMap, head.blocks_in_hdd * sizeof(__s32)); // Take the bytes from the file and store them in iMap for page reading later
}

// Function to read the bytes from the file
void VBox::getByte(char* data, int startingByte, int bytes) {
    
    // startingVPage storese the starting point from which to start looking at pages
    int startingVPage = startingByte >> 20; // Shift the starting byte 20 to the right
    
    // activeBytes stores the current number of bytes the loop has processed from the file
    int activeBytes = 0; // Set the active bytes to 0
    
    // activePage is separate from startingVPage in order to keep the startingPage constant while keeping track of what page we're on as we process pages
    int activePage = startingVPage; // Set the currently active page to the first page to begin
    
    // Loop while the total number of bytes is not 0 when subtracting the bytes currently
    while (bytes - activeBytes > 0) {
        
        int rPage = getPage(activePage); // Get the current page and store it to be worked with
        int readBytes = bytes - activeBytes; // Set the current byte to read from as the next unprocessed byte from bytes
        char* buffer = new char[readBytes]; // Prep a buffer to store all the bytes to process
        
        // If rPage is invalid, fill the buffer with zeroes
        // Otherwise, continue with the process
        if (rPage == -1) {
            memset(buffer, 0, readBytes);
        }
        else {
            // Build the offset based off the data stored in head and the bytes we are processing
            // This will be shifted to the left for formatting
            int offset = head.offset_data + startingByte + ((rPage - activePage) << 20);
            
            _lseek(name, offset, SEEK_SET); // Get the bytes from the file starting at the offset
            read(name, buffer, readBytes); // Store the number of read bytes in the buffer
        }
        
        // Loop through the buffer and store its values in the corresponding values in data
        for (int i = 0; i <= readBytes; i++) {
            data[activeBytes + i] = buffer[i];
        }
        
        activeBytes += readBytes; // Add the number of read bytes to the total bytes we have processed
        activePage++; // Increment to the next page to continue the loop
        
    }
    
    // Once the loop is done, move the pointer back to the beginning of the file
    _lseek(name, 0, SEEK_SET);
    
}

// Simple function that returns the specified value stored in iMap as an int
int VBox::getPage(int pageToRead) {
    return iMap[pageToRead];
}

#endif
