#ifndef VIRTUAL
#define VIRTUAL

#include "Virtual.hpp"
#include <cstring>
#include <unistd.h>

VBox::VBox(int FileDescriptor)
: descriptor(FileDescriptor) {
    /*Constructor code goes here*/
}