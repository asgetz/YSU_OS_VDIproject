#ifndef VIRTUAL_H
#define VIRTUAL_H

// Files to include
#include "ext2_fs.h"

class VBox
{
    private:
<<<<<<< HEAD
	
	__s32* iMap;
=======
	int name; // Stores the file's name
	__s32* iMap; // Stores the pages
>>>>>>> 695acfc90dfc0bcc6d28e6d0749038610e04492d

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
        
        // Put public below private so the compiler knows that vdiHeader exists
        public:
<<<<<<< HEAD
        VBox(int FileDescriptor);
        int descriptor;
=======
        VBox(int FileDescriptor); // Constructor for the class
>>>>>>> 695acfc90dfc0bcc6d28e6d0749038610e04492d
        
        // Simple functions to handle the page and byte reads
        void getByte(char* data, int startingByte, int bytes);
        int getPage(int pageToRead);

	struct vdiHeader head; // Struct is defined above but declared here
};

#endif