/*
 * File:    ext2.cpp
 * Authors: DeLucia, Nicholas
 * 	    Getz,    Alex
 *
 * Created on April 10th, 2018, 6:47 PM
 */

#ifndef EXT2
#define EXT2

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
//#include <assert.h>

#include "ext2.hpp"
#include "ext2_fs.h"
#include "Virtual.cpp"

#define BASE_OFFSET 1024
#define BLOCK_OFFSET(block) (BASE_OFFSET+(block-1)*block_size)

static unsigned int block_size = 0;



/* ext2_fs.h SUPER_MAGIC_NUM = 0xEF53 */
/* Magic number given by Kramer is 0xaa55 ??? */
/* Determine this mixup? */

/**FILE SYSTEM SET UP
 *
 * After read.cpp initializes the struct to mimic the vdi header file, it
 * invokes the ext2 class constructor to initialize the custom file system
 * with the mimic VDI header structure. The constructor initializes in parts:
 *   1. The 1st section
 *
 * @param vBox  =   pointer to the instance of a VDI Header mimic structure
 */
ext2::ext2(VBox* vBox): Box(vBox) {
    int array = sizeof(bootingSect);
    char* data = new char[array];
    Box->getByte(data, 0, array);
    std::memcpy(&mbr, data, array);
    delete data;

    // struct ext2_inode inode;
    // int fd = Box->descriptor;

    std::printf("\nExt2 constructor: Boot Sect initialized.\n\n");


    //////////////////////// MBR SET ////////////////////////
    int partition = -1; int part;
    for(part=0; part<=MBR_PARTITIONS; part++){
        if (mbr.partTable[part].type == 0x83) {partition = part;}
    }
    if(partition==-1){fprintf(stderr,"Not a valid partition\n");exit(1);}
    std::printf("Ext2 constructor: MBR set\n\n");


    /////// LOCATION OF EXT2 FILESYSTEM ACQUIRED /////////////////////////////
    if(mbr.magic!=MAGIC_NUM){std::printf("WARNING!!! WRONG MAGIC NUMBER!");exit(1);}
    std::printf("Ext2 constructor: Apparent EXT2 partition location acquired\n\n");


    ///////////////////// MAGIC NUM CHECK PASSED
    ext2StartingSectByte = mbr.partTable[partition].startingSector * Box->head.sector_size;
    std::printf("Ext2 constructor: Magic number partition location verified\n\n");


    ///////////////// PARTITION TABLE LOADED ///////////////////////////
    data = new char[sizeof(ext2_super_block)];
    Box->getByte(data, ext2StartingSectByte + EXT2_SUPER_OFFSET, sizeof(ext2_super_block));
    std::memcpy(&super, data, sizeof(ext2_super_block));
    delete data;
    if(super.s_magic!=EXT2_SUPER_MAGIC){throw SuperError();}
    else {printf("Ext2 constructor: Passed Superblock magic number check!\n");}
    std::printf("Ext2 constructor: EXT2 Superblock loaded into position\n\n");


    /////////////////// SUPERBLOCK CHECK PASSED ///////////////////////
    block_size = 1024 << super.s_log_block_size;
    blockGroupDescLocation = (super.s_log_block_size>0) ? 1 : 2;
    gCount = 1 + (super.s_blocks_count - 1) / super.s_blocks_per_group;
    sizeBlockDescTable = sizeof(ext2_group_desc) * gCount;
    blockDescTable = new struct ext2_group_desc[gCount];

    std::printf("Ext2 constructor: EXT2 Superblock partition size verified\n");
    std::printf("         cont...: BLOCK GROUP DESCRIPTOR TABLE location affirmed\n\n");


    //////////// BLOCK DESCRIPTOR TABLE LOADED //////////////////////////
    data = new char[sizeBlockDescTable];
    Box->getByte(data, (blockGroupDescLocation * (1024<<super.s_log_block_size)) + ext2StartingSectByte, sizeBlockDescTable);
    std::memcpy(blockDescTable, data, sizeBlockDescTable);
    delete data;
    // read_inode(fd, 2, blockDescTable, &inode);    /* HERE!!!!*/
    // read_dir(fd, &inode, blockDescTable);

    std::printf("Ext2 constructor: BLOCK GROUP DESCRIPTOR TABLE loaded \"[\"!!!!\"]\" \n");
    std::printf("ATTENTION: Ext2 constructor END!\n\n\n");
}









char* ext2::blockRetrieve(int bNum) {
   char* data = new char[1024 << super.s_log_block_size];
   Box->getByte(data, (bNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, (1024 << super.s_log_block_size));
   return data;
}

char* ext2::blockRetrieve(int bNum, int byte) {
   char* data = new char[byte];
   Box->getByte(data, (bNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte, byte);
   return data;
}

char* ext2::blockRetrieve(int bNum, int byte, int offset) {
   char* data = new char[byte];
   Box->getByte(data, (bNum * ( 1024 << super.s_log_block_size)) + ext2StartingSectByte + offset, byte);
   return data;
}


struct ext2_inode ext2::nodeRetrieve(unsigned long long node) {
    int nodeBlockGroup = node / super.s_inodes_per_group;
    unsigned long long nodesInGroup = node % super.s_inodes_per_group;


    bool* nodeMap;
    nodeMap = (bool*) blockRetrieve(blockDescTable[nodeBlockGroup].bg_inode_bitmap, super.s_inodes_per_group / 8);


    int test = nodeMap[nodesInGroup / 8];
    test = (test >> nodesInGroup) & 0x1;

    if(node==0||test==0){printf("iNode Unallocated!\n");
    throw AllocationError();}

    char* data = blockRetrieve(blockDescTable[nodeBlockGroup].bg_inode_table, sizeof(ext2_inode), (super.s_inode_size * (nodesInGroup - 1)));

    struct ext2_inode iNode;
    std::memcpy(&iNode, data, sizeof(ext2_inode));


    return iNode;
}


void ext2::verifySuper()
{
	//take the number of block groups and then read in the superblock copies.

	printf("Block    Inodes    Blocks    Reserved     First     Log Block   Log Frag   Blocks Per\n"
				 "Group    Count     Count     Blocks     Data Block     Size         Size        Group\n"
			   "-------------------------------------------------------------------------------------\n");


	for (unsigned int i = 0; i < gCount; i++)
	{
        bool pwrCheck;
        pwrCheck = (i!=0 && 3486784401u%i==0) || (i!=0 && 95367431640625u%i==0) || (i!=0 && 79792266297612001u%i==0);
		if (pwrCheck || i==0)
		{
			ext2_super_block *retrieve;
			unsigned int offset = 0;
			if (i == 0 || blockGroupDescLocation - 1)
				offset = EXT2_SUPER_OFFSET;

			retrieve = (ext2_super_block *) blockRetrieve(i * super.s_blocks_per_group, sizeof(ext2_super_block), offset);

			printf("%5u %9u %9u %11u %12u %11u %10u %12u\n",
						i,
						retrieve->s_inodes_count,
						retrieve->s_blocks_count,
						retrieve->s_r_blocks_count,
						retrieve->s_first_data_block,
						retrieve->s_log_block_size,
						retrieve->s_log_cluster_size,
						retrieve->s_blocks_per_group);
			delete retrieve;
		}
	}

	printf("\n\n"
				 "Block    Frags Per    Magic     Minor Revision    Creator    Revision    First    Inode\n"
				 "Group    Group                  Level             OS         Level       Inode    Size \n"
			 	 "---------------------------------------------------------------------------------------\n");

	for (unsigned int i = 0; i < gCount; i++)
	{
        bool pwrCheck;
        pwrCheck = (i!=0 && 3486784401u%i==0) || (i!=0 && 95367431640625u%i==0) || (i!=0 && 79792266297612001u%i==0);
		if (pwrCheck || i == 0)
		{
			ext2_super_block *retrieve;
			unsigned int offset = 0;
			if (i == 0 || blockGroupDescLocation - 1)
				offset = EXT2_SUPER_OFFSET;

			retrieve = (ext2_super_block *) blockRetrieve(i*super.s_blocks_per_group, sizeof(ext2_super_block), offset);

			printf("%5u %12u %8u %18u %10u %11u %8u %8u\n",
						i,
						retrieve->s_clusters_per_group,
						retrieve->s_magic,
						retrieve->s_minor_rev_level,
						retrieve->s_creator_os,
						retrieve->s_rev_level,
						retrieve->s_first_ino,
						retrieve->s_inode_size);

			delete retrieve;
		}
	}
	printf("\n\n");
	// return 0;
}

void ext2::verify_bGroupTable()
{
	printf("Group    Block     Inode      Inode    Free      Free        Used\n"
         "         Bitmap    Bitmap     Table    Blocks    Inodes      Dirs\n"
         "-----------------------------------------------------------------\n");
	for (unsigned int i = 0; i < gCount; i++)
	{
		printf("%5u %9u %9u %9u %9u %9u %9u\n",
					i,
					blockDescTable[i].bg_block_bitmap,
					blockDescTable[i].bg_inode_bitmap,
					blockDescTable[i].bg_inode_table,
					blockDescTable[i].bg_free_blocks_count,
					blockDescTable[i].bg_free_inodes_count,
					blockDescTable[i].bg_used_dirs_count);
	}
	printf("\n\n");
	// return 0;
}





int ext2::verifyNodes(unsigned long long iNum){
    try {
        struct ext2_inode iNode = this->nodeRetrieve(iNum);
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
                    this->dirRetrieve(iNode.i_block[i], 0);
                }
            }
        }
    }
    catch(AllocationError)
	{
        printf("Inode is not allocated");
        return 1;
	}

    return 0;
}

struct ext2_dir_entry_2 ext2::dirRetrieve(unsigned long block, unsigned long offset) {

    std::vector<struct ext2_dir_entry_2> entries;
    struct ext2_dir_entry_2 test;
    unsigned int totalLength = 0;

    std::memcpy(&test, this->blockRetrieve(block, *(this->blockRetrieve(block, 2, 4)), 0), *(this->blockRetrieve(block, 2, 4)));

    printf("Block: 0x%x\nlength: %x\nFileType: %x\nName: %s\n", block, test.name_len,test.file_type, test.name);

    totalLength += test.rec_len;
    entries.push_back(test);
    bool addEntries = totalLength - (1024 << super.s_log_block_size);
    while (addEntries) {
        char* firstLength = this->blockRetrieve(block, 2, 4 + totalLength);
        unsigned int secondLength = (unsigned int) *firstLength;

        if (secondLength >= (1024 << super.s_log_block_size)) {break;}

        printf("length2: %u\n", secondLength);
        delete firstLength;
        std::memcpy(&test, this->blockRetrieve(block, secondLength, totalLength), secondLength);

        printf("%u\n", totalLength);
        totalLength += test.rec_len;
        addEntries = totalLength - (1024 << super.s_log_block_size);
        entries.push_back(test);
    }return test;
}

#endif
