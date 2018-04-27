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
    
    for(int part=0; part<=MBR_PARTITIONS; part++){
        if (mbr.partTable[part].type == 0x83) {ext2_part = part;}
    }
    if(ext2_part==-1){throw UndefinedPartitionError();}
//    else {printf("\n\nPassed partition check!\n");}
    assert(ext2_part>=0 && ext2_part<=4);
    if(mbr.magic!=MAGIC_NUM){throw FalseSectorError();}
//    else {printf("\n\nPassed Magic number check!\n");}
    

    
    ext2StartingSectByte = mbr.partTable[ext2_part].startingSector * Box->head.sector_size;
    
    
    data = new char[sizeof(ext2_super_block)];
    Box->getByte(data, ext2StartingSectByte + EXT2_SUPER_OFFSET, sizeof(ext2_super_block));
    
    std::memcpy(&super, data, sizeof(ext2_super_block));
    delete data;
    
    if(super.s_magic!=EXT2_SUPER_MAGIC){throw FalseSuperError();}
//    else {printf("\n\nPassed Superblock magic number check!\n");}
    
    super.s_log_block_size>0? blockGroupDescLocation=1:
        blockGroupDescLocation=2;
    
    
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

void ext2::setBlock(int blockNum, char* data) {
    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, (1024 << super.s_log_block_size));
}

void ext2::setBlock(int blockNum, int byte, char* data) {
    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, byte);
}

void ext2::setBlock(int blockNum, int byte, int offset, char* data) {
    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte + offset, byte);
}

struct ext2_inode ext2::getNode(unsigned long long node) {
    int nodeBlockGroup = node / super.s_inodes_per_group;
    unsigned long long nodesInGroup = node % super.s_inodes_per_group;
    
    
    bool* nodeMap;
    nodeMap = (bool*) getBlock(blockDescTable[nodeBlockGroup].bg_inode_bitmap, super.s_inodes_per_group / 8);
    
    
    int test = nodeMap[nodesInGroup / 8];
    test = (test >> nodesInGroup) & 0x1;
    
    if(node==0||test==0){printf("iNode Unallocated!\n");
    throw iNodeAllocationError();}
    
    char* data = getBlock(blockDescTable[nodeBlockGroup].bg_inode_table, sizeof(ext2_inode), (super.s_inode_size * (nodesInGroup - 1)));
    
    struct ext2_inode iNode;
    std::memcpy(&iNode, data, sizeof(ext2_inode));
    
    return iNode;
    
}

bool isPowerof357(unsigned int number);
void ext2::verifySuper() {
    //take the number of block groups and then read in the superblock copies.

	printf("Block    Inodes    Blocks    Reserved     First     Log Block   Log Frag   Blocks Per\n"
				 "Group    Count     Count     Blocks     Data Block     Size         Size        Group\n"
			   "-------------------------------------------------------------------------------------\n");


    for (unsigned int i = 0; i < gCount; i++){
        if (isPowerof357(i) || i == 0){
            ext2_super_block *copy;
            unsigned int offset = 0;
            if(i==0 || blockGroupDescLocation-1)
//                offset = EXT2_SUPER_BLOCK_OFFSET;
                offset = EXT2_SUPER_OFFSET;

            copy = (ext2_super_block *) getBlock(i * super.s_blocks_per_group, sizeof(ext2_super_block), offset);

            printf("%5u %9u %9u %11u %12u %11u %10u %12u\n",
                i,
                copy->s_inodes_count,
                copy->s_blocks_count,
                copy->s_r_blocks_count,
                copy->s_first_data_block,
                copy->s_log_block_size,
                copy->s_log_cluster_size,
                copy->s_blocks_per_group);
            delete copy;
        }
    }

    printf("\n\n"
        "Block    Frags Per    Magic     Minor Revision    Creator    Revision    First    Inode\n"
        "Group    Group                  Level             OS         Level       Inode    Size \n"
        "---------------------------------------------------------------------------------------\n");

    for (unsigned int i = 0; i < gCount; i++){
        if (isPowerof357(i) || i == 0){
            ext2_super_block *copy;
            unsigned int offset = 0;
            if(i==0 || blockGroupDescLocation-1){
//                offset = EXT2_SUPER_BLOCK_OFFSET;
                offset = EXT2_SUPER_OFFSET;
            }

            copy = (ext2_super_block *) getBlock(i*super.s_blocks_per_group, sizeof(ext2_super_block), offset);

            printf("%5u %12u %8u %18u %10u %11u %8u %8u\n",
                i,
                copy->s_clusters_per_group,
                copy->s_magic,
                copy->s_minor_rev_level,
                copy->s_creator_os,
                copy->s_rev_level,
                copy->s_first_ino,
                copy->s_inode_size);

            delete copy;
        }
    }printf("\n\n");return;
}

void ext2::bGroupTableDump() {
    printf("Group    Block     Inode      Inode    Free      Free        Used\n"
         "         Bitmap    Bitmap     Table    Blocks    Inodes      Dirs\n"
         "-----------------------------------------------------------------\n");
	for (unsigned int i = 0; i < gCount; i++){
            printf("%5u %9u %9u %9u %9u %9u %9u\n",
                i,
                blockDescTable[i].bg_block_bitmap,
                blockDescTable[i].bg_inode_bitmap,
                blockDescTable[i].bg_inode_table,
                blockDescTable[i].bg_free_blocks_count,
                blockDescTable[i].bg_free_inodes_count,
                blockDescTable[i].bg_used_dirs_count);
	}printf("\n\n");return;
}

int ext2::verifyNodes(unsigned long long nodeNum) {
    try {
        struct ext2_inode iNode = this->getNode(nodeNum);
//        printf("Starting from here: ");
        
        if (iNode.i_mode & EXT2_S_IFREG) {
            printf("inode: %u\n"
                "Size: %u\n"
                "links: %u\n",
                iNode.i_mode,
                iNode.i_size,
                iNode.i_links_count);
            return 0;
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
    
    printf("Block: 0x%x\nlength: %x\nFileType: %x\nName: %s\n", block, test.name_len,test.file_type, test.name);
    
    totalLength += test.rec_len;
    entries.push_back(test);
    bool addEntries = totalLength - (1024 << super.s_log_block_size);
    while (addEntries) {
        char* firstLength = this->getBlock(block, 2, 4 + totalLength);
        unsigned int secondLength = (unsigned int) *firstLength;
        
        if (secondLength >= (1024 << super.s_log_block_size)) {break;}
        
        printf("length2: %u\n", secondLength);
        delete firstLength;
        std::memcpy(&test, this->getBlock(block, secondLength, totalLength), secondLength);
        
//        printf("%u\n", totalLength);
        totalLength += test.rec_len;
        addEntries = totalLength - (1024 << super.s_log_block_size);
        entries.push_back(test);
    }return test;
}

bool isPowerof357(unsigned int number)
{
	//3^19, 5^19, and 7^19 respectively. Avoids the pesky while loop by seeing if max unsigned int is divisible evenly.
	return (number != 0 && 3486784401u % number == 0) || (number != 0 && 95367431640625u % number == 0) || (number != 0 && 79792266297612001u % number == 0);
}


#endif