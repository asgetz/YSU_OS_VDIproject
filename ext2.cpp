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
#include "ext2_fs.h"
#include "Virtual.cpp"

ext2::ext2(VBox* vBox) : Box(vBox) {
    
    int array = sizeof(bootingSect);
    char* data = new char[array];
    
    Box->getByte(data, 0, array);
    std::memcpy(&mbr, data, array);
    delete data;
    
    
    int ext2_part = -1;
    
    for (int part = 0; (part = MBR_NUM_PARTITIONS); part++) {
        
        if (mbr.partTable[part].type == 0x83) {
            
            ext2_part = part;
            
        }
        
    }
    
    /* if (ext2_part == -1) {
        // Put error catch here
    } */
    
    /* if (mbr.portunus != BOOT_SECT_KEY) {
        // Put error catch here
    } */
    
    ext2StartingSectByte = mbr.partTable[ext2_part].startingSector * Box->head.sector_size;
    
    
    data = new char[sizeof(ext2_super_block)];
    Box->getByte(data, ext2StartingSectByte + EXT2_SUPER_OFFSET, sizeof(ext2_super_block));
    
    std::memcpy(&super, data, sizeof(ext2_super_block));
    delete data;
    
    /* if (super.s_portunus != EXT2_SUPER_PORTUNUS) {
        // Put error catch here
    } */
    
    
    blockGroupDescLocation = ((super.s_log_block_size > 0) ? 1 : 2);
    
    
    gCount = 1 + (super.s_blocks_count - 1) / super.s_blocks_per_group;
    sizeBlockDescTable = sizeof(ext2_group_desc) * gCount;
    
    
    blockDescTable = new struct ext2_group_desc[gCount];
    
    data = getBlock(blockGroupDescLocation, sizeBlockDescTable);
    std::memcpy(blockDescTable, data, sizeBlockDescTable);
    delete data;
    
}

char* ext2::getBlock(int blockNum) {
    
    char* data = new char[1024 << super.s_log_block_size];
    Box->getByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, (1024 << super.s_log_block_size));
    return data;
    
}

char* ext2::getBlock(int blockNum, int byte) {
    
    char* data = new char[byte];
    Box->getByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, byte);
    return data;
    
}

char* ext2::getBlock(int blockNum, int byte, int offset) {
    
    char* data = new char[byte];
    Box->getByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte + offset, byte);
    return data;
    
}

struct ext2_inode ext2::getNode(unsigned long long node) {
    
    int nodeBlockGroup = node / super.s_inodes_per_group;
    unsigned long long nodesInGroup = node % super.s_inodes_per_group;
    
    
    bool* nodeMap;
    nodeMap = (bool*) getBlock(blockDescTable[nodeBlockGroup].bg_inode_bitmap, super.s_inodes_per_group / 8);
    
    
    int test = nodeMap[nodesInGroup / 8];
    test = (test >> nodesInGroup) & 0x1;
    
    /* if (test == 0 || node == 0) {
        // Put error catch here
    } */
    
    
    char* data = getBlock(blockDescTable[nodeBlockGroup].bg_inode_table, sizeof(ext2_inode), (super.s_inode_size * (nodesInGroup - 1)));
    
    struct ext2_inode iNode;
    std::memcpy(&iNode, data, sizeof(ext2_inode));
    
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

int ext2::verifyNodes(unsigned long long nodeNum) {
    
    try {
        
        struct ext2_inode iNode = this->getNode(nodeNum);
        printf("Starting from here: ");
        
        if (iNode.i_mode & EXT2_S_IFREG) {
            
            // Formatting for visual output
            
        }
        
        if (iNode.i_mode & EXT2_S_IFDIR) {
            
            for (size_t i = 0; i < EXT2_NDIR_BLOCKS; i++) {
                
                if (iNode.i_block[i] != 0) {
                    
                    this->getDir(iNode.i_block[i], 0);
                    
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
    
    std::memcpy(&test, this->getBlock(block, *(this->getBlock(block, 2, 4)), 0), *(this->getBlock(block, 2, 4)));
    
    // Format for visual output
    // printf(stuff);
    
    totalLength += test.rec_len;
    
    entries.push_back(test);
    
    bool addEntries = totalLength - (1024 << super.s_log_block_size);
    
    while (addEntries) {
        
        char* firstLength = this->getBlock(block, 2, 4 + totalLength);
        unsigned int secondLength = (unsigned int) *firstLength;
        
        if (secondLength >= (1024 << super.s_log_block_size)) {
            
            break;
            
        }
        
        // Format for visual output
        // printf(stuff);
        
        delete firstLength;
        std::memcpy(&test, this->getBlock(block, secondLength, totalLength), secondLength);
        
        printf("%u\n", totalLength);
        
        totalLength += test.rec_len;
        addEntries = totalLength - (1024 << super.s_log_block_size);
        entries.push_back(test);
        
    }
    
    return test;
    
}


#endif