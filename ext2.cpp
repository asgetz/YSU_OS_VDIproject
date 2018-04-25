/* 
 * File:    ext2.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on April 10th, 2018, 6:47 PM
 */

#ifndef EXT2
#define EXT2

#include <cstring>
#include <stdio.h>
#include <assert.h>
#include <vector>

#include "ext2.hpp"
#include "Virtual.cpp"

ext2::ext2(VBox* Box) : virtualBox(Box) {
    
    int array = sizeof(bootingSect);
    char* data = new char[array];
    
    Box->getByte(data, 0, array);
    std::memcpy(&mbr, data, array);
    delete data;
    
    
    int ext2_part = -1;
    
    for (int part = 0; part <= MBR_NUM_PART; part++) {
        
        if (mbr.partTable[partNum].type == 0x83) {
            
            ext2_part = part;
            
        }
        
    }
    
    /* if (ext2_part == -1) {
        // Put error catch here
    } */
    
    /* if (mbr.portunus != BOOT_SECT_KEY) {
        // Put error catch here
    } */
    
    ext2StartingSectByte = mbr.partTable[ext2_part].startingSect * Box->header.sect_size;
    
    
    data = new char[sizeof(ext2_super)];
    Box->getByte(data, ext2StartingSectByte + EXT2_SUPER_OFFSET, sizeof(ext2_super));
    
    std::memcpy(&super, data, sizeof(ext2_super));
    delete data;
    
    /* if (super.s_portunus != EXT2_SUPER_PORTUNUS) {
        // Put error catch here
    } */
    
    
    blockGroupDescLocation = ((super.sLogBSize > 0) ? 1 : 2);
    
    
    gCount = 1 + (super.sBlockCount - 1) / super.sBlocksInGroup;
    sizeBlockDescTable = sizeof(ext2_group_desc) * gCount;
    
    
    blockDescTable = new struct ext2_group_desc[gCount];
    
    data = getBlock(blockGroupDescLocation, sizeBlockDescTable);
    std::memcpy(blockDescTable, data, sizeBlockDescTable);
    delete data;
    
}

char* ext2::getBlock(int blockNum) {
    
    char* data = new char[1024 << super.sLogBSize];
    Box->getBytes(data, (blockNum * ( 1024 << super.sLogBSize)) + ext2StartingSectByte, (1024 << super.sLogBSize));
    return data;
    
}

char* ext2::getBlock(int blockNum, int byte) {
    
    char* data = new char[byte];
    Box->getBytes(data, (blockNum * ( 1024 << super.sLogBSize)) + ext2StartingSectByte, byte);
    return data;
    
}

char* ext2::getBlock(int blockNum, int byte, int offset) {
    
    char* data = new char[byte];
    Box->getBytes(data, (blockNum * ( 1024 << super.sLogBSize)) + ext2StartingSectByte + offset, byte);
    return data;
    
}

struct ext2_inode ext2::getNode(unsigned long long node) {
    
    int nodeBlockGroup = node / super.sNodesInGroup;
    unsigned long long nodesInGroup = node % super.sNodesInGroup;
    
    
    bool* nodeMap;
    nodeMap = (bool*) getBlock(blockDescTable[nodeBlockGroup].bgNodeBitmap, super.sNodesInGroup / 8);
    
    
    int test = nodeMap[nodesInGroup / 8];
    test = (test >> nodesInGroup) & 0x1;
    
    /* if (test == 0 || node == 0) {
        // Put error catch here
    } */
    
    
    char* data = getBlock(blockDescTable[nodeBlockGroup].bgNodeTable, sizeof(ext2_inode), (super.sNodeSize * (nodesInGroup - 1)));
    
    struct ext2_inode iNode;
    std::memcpy(&iNode, data, sizeof(et2_inode));
    
    return iNode;
    
}

void ext2::verifySuper() {
    
    // Mainly formatting for visual output
    
    return;
    
}

void ext2::bGroupTableDump() {
    
    // Mainly formatting for visual output
    
    return;
    
}

int verifyNodes(unsigned long long nodeNum) {
    
    try {
        
        struct ext2_inode iNode = this->getNode(nodeNum);
        printf("Starting from here: ");
        
        if (iNode.iMode & EXT2_S_IFREG) {
            
            // Formatting for visual output
            
        }
        
        if (iNode.iMode & EXT2_S_IFDIR) {
            
            for (size_t i = 0; i < EXT2_NDIR_BLOCKS; i++) {
                
                if (iNode.iBlock[i] != 0) {
                    
                    this->getDir(iNode.iBlock[i], 0);
                    
                }
                
            }
            
        }
        
    }
    catch (iNodeAllocationError) {
        
        printf("The iNode has not been allocated.");
        return 1;
        
    }
    
    return 0;
    
}

struct ext2_dir_entry_2 ext2::getDir(unsigned long block, unsigned long offset) {
    
    std::vector<struct ext2_dir_entry_2> entries;
    struct ext2_dir_entry_2 test;
    unsigned int totalLength = 0;
    
    std::memcpy(&temp, this->getBlock(block, *(this->getBlock(block, 2, 4)), 0), *(this->getBlock(block, 2, 4)));
    
    // Format for visual output
    // printf(stuff);
    
    totalLength += temp.rec_len;
    
    entries.push_back(test);
    
    bool addEntries = totalLength - (1024 << super.sLogBSize);
    
    while (addEntries) {
        
        char* firstLength = this->getBlock(block, 2, 4 + totalLength);
        unsigned int secondLength = (unsigned int) *firstLength;
        
        if (secondLength >= (1024 << super.sLogBSize)) {
            
            break;
            
        }
        
        // Format for visual output
        // printf(stuff);
        
        delete firstLength;
        std::memcpy(&test, this->getBlock(block, secondLength, totalLength), secondLength);
        
        printf("%u\n", totalLength);
        
        totalLength += test.rec_len;
        addEntries = totalLength - (1024 << super.sLogBSize);
        entries.push_back(test);
        
    }
    
    return test;
    
}


#endif