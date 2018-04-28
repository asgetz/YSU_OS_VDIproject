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

//#define BLOCK_OFFSET(block) (EXT2_SUPER_OFFSET+(block-1)*block_size)

ext2::ext2(VBox* vBox) : Box(vBox) {
    int array = sizeof(bootingSect);
    char* data = new char[array];
    Box->getByte(data, 0, array);
    std::memcpy(&mbr, data, array);
    delete data;
    //////////////////////// MBR SET ////////////////////////
    
    int ext2_part = -1;
    
    for(int part=0; part<=MBR_PARTITIONS; part++){
        if (mbr.partTable[part].type == 0x83) {ext2_part = part;}
    }
    if(ext2_part==-1){throw UndefinedPartitionError();}
//    else {printf("\n\nPassed partition check!\n");}
    /////// LOCATION OF EXT2 FILESYSTEM ACQUIRED /////////////////////////////
    
    if(mbr.magic!=MAGIC_NUM){throw FalseSectorError();}
//    else {printf("\n\nPassed Magic number check!\n");}
    ///////////////////// MAGIC NUM CHECK PASSED
    

    
    ext2StartingSectByte = mbr.partTable[ext2_part].startingSector * Box->head.sector_size;
    ///////////////// PARTITION TABLE LOADED ///////////////////////////
    
    
    data = new char[sizeof(ext2_super_block)];
    Box->getByte(data, ext2StartingSectByte + EXT2_SUPER_OFFSET, sizeof(ext2_super_block));
    
    std::memcpy(&super, data, sizeof(ext2_super_block));
    delete data;
    
    if(super.s_magic!=EXT2_SUPER_MAGIC){throw FalseSuperError();}
//    else {printf("\n\nPassed Superblock magic number check!\n");}
    /////////////////// SUPERBLOCK CHECK PASSED ///////////////////////
    
    
    super.s_log_block_size>0? blockGroupDescLocation=1:
        blockGroupDescLocation=2;
    
    
    gCount = 1 + (super.s_blocks_count - 1) / super.s_blocks_per_group;
    sizeBlockDescTable = sizeof(ext2_group_desc) * gCount;
    
    
    blockDescTable = new struct ext2_group_desc[gCount];
    //////////// BLOCK DESCRIPTOR TABLE LOADED //////////////////////////
    
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

//void ext2::setBlock(int blockNum, char* data) {
//    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, (1024 << super.s_log_block_size));
//}

//void ext2::setBlock(int blockNum, int byte, char* data) {
//    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, byte);
//}

//void ext2::setBlock(int blockNum, int byte, int offset, char* data) {
//    Box->setByte(data, (blockNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte + offset, byte);
//}

////////////////// THIS IS WHERE KRAMER'S ALGORITHM STARTS TO COME IT ////////
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

//static 
//void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
//{
//	lseek(fd, BLOCK_OFFSET(group->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), 
//	      SEEK_SET);
//	read(fd, inode, sizeof(struct ext2_inode));
//} /* read_inode() */
//
//
//static void read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group)
//{
//	void *block;
//
//	if (S_ISDIR(inode->i_mode)) {
//		struct ext2_dir_entry_2 *entry;
//		unsigned int size = 0;
//
//		if ((block = malloc(block_size)) == NULL) { /* allocate memory for the data block */
//			fprintf(stderr, "Memory error\n");
//			close(fd);
//			exit(1);
//		}
//
//		lseek(fd, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
//		read(fd, block, block_size);                /* read block from disk*/
//
//		entry = (struct ext2_dir_entry_2 *) block;  /* first entry in the directory */
//                /* Notice that the list may be terminated with a NULL
//                   entry (entry->inode == NULL)*/
//		while((size < inode->i_size) && entry->inode) {
//			char file_name[EXT2_NAME_LEN+1];
//			memcpy(file_name, entry->name, entry->name_len);
//			file_name[entry->name_len] = 0;     /* append null character to the file name */
//			printf("%10u %s\n", entry->inode, file_name);
//			entry = (void*) entry + entry->rec_len;
//			size += entry->rec_len;
//		}
//
//		free(block);
////                copy = (ext2_super_block *) getBlock(i * super.s_blocks_per_group, sizeof(ext2_super_block), offset);///////
//	}
//} /* read_dir() */

//int ext2::verifyNodes(unsigned long long nodeNum) {
//    try {
//        struct ext2_inode iNode = this->getNode(nodeNum);
////        printf("Starting from here: ");
//        
//        if (iNode.i_mode & EXT2_S_IFREG) {
//            printf("inode: %u\n"
//                "Size: %u\n"
//                "links: %u\n",
//                iNode.i_mode,
//                iNode.i_size,
//                iNode.i_links_count);
//            return 0;
//        }
//        
//        if (iNode.i_mode & EXT2_S_IFDIR) {
//            for (size_t i = 0; i < EXT2_NDIR_BLOCKS; i++) {
//                if (iNode.i_block[i] != 0) {
//                    this->getDir(iNode.i_block[i], 0);
//                }
//            }
//        }
//    }

//struct ext2_dir_entry_2 ext2::getDir(unsigned long block, unsigned long offset) {
//    
//    std::vector<struct ext2_dir_entry_2> entries;
//    struct ext2_dir_entry_2 test;
//    unsigned int totalLength = 0;
//    
//    std::memcpy(&test, this->getBlock(block, *(this->getBlock(block, 2, 4)), 0), *(this->getBlock(block, 2, 4)));
//    
//    printf("Block: 0x%x\nlength: %x\nFileType: %x\nName: %s\n", block, test.name_len,test.file_type, test.name);
//    
//    totalLength += test.rec_len;
//    entries.push_back(test);
//    bool addEntries = totalLength - (1024 << super.s_log_block_size);
//    while (addEntries) {
//        char* firstLength = this->getBlock(block, 2, 4 + totalLength);
//        unsigned int secondLength = (unsigned int) *firstLength;
//        
//        if (secondLength >= (1024 << super.s_log_block_size)) {break;}
//        
//        printf("length2: %u\n", secondLength);
//        delete firstLength;
//        std::memcpy(&test, this->getBlock(block, secondLength, totalLength), secondLength);
//        
////        printf("%u\n", totalLength);
//        totalLength += test.rec_len;
//        addEntries = totalLength - (1024 << super.s_log_block_size);
//        entries.push_back(test);
//    }return test;
//}
#endif