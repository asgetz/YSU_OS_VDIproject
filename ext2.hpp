#ifndef EXT2_H
#define EXT2_H

#include "ext2_fs.h"
#include "Virtual.hpp"
#include "mbr.h"



class ext2 {

public:
    ext2(VBox* vBox);

    char* blockRetrieve(int bNum);
    char* blockRetrieve(int bNum, int byte);
    char* blockRetrieve(int bNum, int byte, int offset);

    struct ext2_inode nodeRetrieve(unsigned long long node);
    struct ext2_dir_entry_2 dirRetrieve(unsigned long block, unsigned long offset);

    void verifySuper();
    void verify_bGroupTable();
    int verifyNodes(unsigned long long = 2);

private:
    VBox* Box;
    const static int EXT2_SUPER_OFFSET = 1024;
    struct bootingSect mbr;
    unsigned int ext2StartingSectByte;
    struct ext2_super_block super;
    struct ext2_group_desc* blockDescTable;
    unsigned int gCount;
    unsigned int sizeBlockDescTable;
    long long blockGroupDescLocation;

};

class AllocationError {};
class PartitionError {};
class SectorError {};
class SuperError {};


#define EXT2_S_IFSOCK   0xC000
#define EXT2_S_IFLNK    0xA000
#define EXT2_S_IFREG    0x8000
#define EXT2_S_IFBLK    0x6000
#define EXT2_S_IFDIR    0x4000
#define EXT2_S_IFCHR    0x2000
#define EXT2_S_IFIFO    0x1000

#define EXT2_S_ISUID    0x0800
#define EXT2_S_ISGID    0x0400
#define EXT2_S_ISVTX    0x0200

#define EXT2_S_IRUSR    0x0100
#define EXT2_S_IWUSR    0x0080
#define EXT2_S_IXUSR    0x0040
#define EXT2_S_IRGRP    0x0020
#define EXT2_S_IWGRP    0x0010
#define EXT2_S_IXGRP    0x0008
#define EXT2_S_IROTH    0x0004
#define EXT2_S_IWOTH    0x0002
#define EXT2_S_IXOTH    0x0001

#endif
