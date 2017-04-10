#ifndef	FILES_H
#define FILES_H

#include "types.h"
#include "syscall.h"
#define TOTAL_DENTRY 64
#define BLOCK_SIZE 4096
#define BLOCK_ADDR_SIZE 1024
#define STAT_SIZE 64
#define MAX_FILE_CHAR 32
#define BYTE 8
#define FOUR_BYTE 32
#define STAT_ADDR_SIZE 16
#define POS_FILE_TYPE 8
#define POS_FILE_INODE 9


typedef struct dentry{
    uint8_t file_name[MAX_FILE_CHAR+1];
    uint32_t file_type;
    uint32_t inode;
    uint8_t reserved[24];//put reserved bytes of chars mentioned in documents
} dentry_t;

typedef struct boot_block{
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[52]; //put reserved bytes of chars mentioned in documents
} boot_block_t;


/*Variables to be used in functions*/
boot_block_t my_boot_block;
dentry_t my_dentry[TOTAL_DENTRY];
uint32_t* inodes;	//starting address of inodes
uint32_t* data_blocks; // starting address of data blocks
uint32_t* my_file_sys; // starting address of the file systems

/*functions to be used in file systems*/
void file_open(uint32_t* add_start);
int32_t file_close(void);
int32_t file_write(const uint8_t* fname, uint32_t offset, uint8_t* buf,uint32_t length);
int32_t file_read(const uint8_t* fname, uint32_t offset, uint8_t* buf,uint32_t length);
int32_t write_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length);
int32_t dir_open(void);
int32_t dir_close(void);
int32_t dir_write(void);
int32_t dir_read(int8_t* buf);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length);

#endif
