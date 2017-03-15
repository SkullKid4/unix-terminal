#include "types.h"
#define TOTAL_DENTRY 64
#define BLOCK_SIZE 4096
#define MAX_FILE 32
#define BYTE 8

//static unsigned int curr_num_dentries=0;
typedef struct data_block{
    uint8_t data[BLOCK_SIZE];
} data_block_t;

typedef struct inode{
    unsigned char length;
    unsigned char padding[3];	//padding length info to 4B
    uint8_t data_block_index[BLOCK_SIZE-1];
} inode_t;

typedef struct dentry{
    uint8_t file_name[MAX_FILE];
    uint32_t file_type;
    uint32_t inode;
    uint8_t reserved[24];//put reserved bytes of chars mentioned in documents
} dentry_t;

typedef struct boot_block{
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52]; //put reserved bytes of chars mentioned in documents
    dentry_t my_dentry[TOTAL_DENTRY];
} boot_block_t;


typedef struct file_sys{
    boot_block_t my_boot_block;
    uint8_t* inodes;
    uint8_t* data_blocks;
} file_sys_t;

file_sys_t my_file_sys;
void fs_open(void);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length);
