#ifndef VIRTUAL_H
#define VIRTUAL_H

#include "ext2_fs.h"

class VBox
{
    protected:

    private:
	int descriptor;
	__s32* iMap;

        struct __attribute__((packed)) vdiHeader {  //!< Member variable "VDIheader"
            /* This VirtualHeader will mirror the structure of the VDI header byte-for-byte.
               That means that there must be a variable for each valuable & unique piece of information
               within the VDI header, and that variables will be needed for certain chunks of
               information deemed less valuable or for chunks of info that is useless when separated.
               NOTE TO SELF: the structure is described in detail in the VirtualBox.com forum post
            */

            char description[0x40]; //First 40 bytes of the VDI header will go in here
            __u32 image_signature;
            __u32 version;
            __u32 header_size;
            __u32 image_type;
            char misc[0x104]; // 104 bytes of miscellaneous data
            __u32 offset_blocks;
            __u32 offset_data;
            __u32 cylinders;
            __u32 heads;
            __u32 sectors;
            __u32 sector_size;
            __u32 garbage;
            __u64 disk_size_bytes;
            __u32 block_size;
            __u32 block_extra_data;
            __u32 blocks_in_hdd;
            __u32 blocks_allocated;
            __u32 uuid_vdi[4];
            __u32 uuid_link[4];
            __u32 uuid_parent[4];
                
	};
        
        public:
        VBox(int FileDescriptor);
        
        void getByte(char* data, int startingByte, int bytes);
//        void setByte(char* data, int startingByte, int bytes);
        int getPage(int page);
//        void setPage(int page, int insert);

	struct vdiHeader head; //Declaring the struct, but not defining it.
};

#endif // VIRTUAL_H