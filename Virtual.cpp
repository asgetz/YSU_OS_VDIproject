/* 
 * File:    Virtual.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on January 31, 2018, 8:44 PM
 */

#ifndef VIRTUAL
#define VIRTUAL

#include "Virtual.hpp"
#include <cstring>
#include <unistd.h>

VBox::VBox(int FileDescriptor)
: descriptor(FileDescriptor) {
    
    lseek(descriptor, 0, SEEK_SET);
    
    read(descriptor, &head, sizeof(head));
    
    lseek(descriptor, head.offset_blocks, SEEK_SET);
    
    iMap = new __s32[head.blocks_in_hdd];
    read(descriptor, iMap, head.blocks_in_hdd * sizeof(__s32));
}

void VBox::getByte(char* data, int startingByte, int bytes) {
    
    int byteStorage = bytes;
    
    int startingVPage = startingByte >> 20;
    
    int activeBytes = 0;
    
    int activePage = startingVPage;
    
    while (byteStorage - activeBytes > 0) {
        
        int rPage = getPage(activePage);
        int readBytes = byteStorage - activeBytes;
        char* buffer = new char[readBytes];
        
        if (rPage == -1) {
            memset(buffer, 0, readBytes);
        }
        else {
            int offset = head.offset_data + startingByte + ((rPage - activePage) << 20);
            
            lseek(descriptor, offset, SEEK_SET);
            read(descriptor, buffer, readBytes);
        }
        
        for (int i = 0; i <= readBytes; i++) {
            data[activeBytes + i] = buffer[i];
        }
        
        activeBytes += readBytes;
        activePage++;
        
    }
    
    lseek(descriptor, 0, SEEK_SET);
    
}

void VBox::setByte(char* data, int startingByte, int bytes) {
    int byteStorage = bytes;
    int activePage = startingByte >> 20;
    int activeBytes = 0;
    
    while (byteStorage - activeBytes > 0) {
        
        int readBytes = byteStorage - activeBytes;
        
        for (int i = 0; i <= readBytes; i++) {
            setPage(activePage + i) = data[i];
        }
        
        activeBytes += readBytes;
        activePage++;
    }
    
    return;
}

int VBox::getPage(int page) {
    return iMap[page];
}

void VBox::setPage(int page, int insert) {
    iMap[page] = insert;
    return;
}

#endif
