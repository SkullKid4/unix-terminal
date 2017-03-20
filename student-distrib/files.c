#include "files.h"
#include "lib.h"

void file_open(uint32_t* add_start, uint32_t* add_end){
	my_file_sys=add_start;
	memcpy((void*)(&my_boot_block),(void*)my_file_sys,STAT_SIZE);
	inodes=my_file_sys+BLOCK_ADDR_SIZE;
	data_blocks=my_file_sys+BLOCK_ADDR_SIZE*(1+my_boot_block.num_inodes);
	
	int i;
	uint32_t* curr=my_file_sys+16;//calculate starting address of the statistics
	for(i=0;i<my_boot_block.num_dentries;i++){
		memcpy(&(my_dentry[i].file_name),curr,MAX_FILE_CHAR);//copy each dentry info to dentry struct
		memcpy(&(my_dentry[i].file_type),curr+8,4);
		memcpy(&(my_dentry[i].inode),curr+9,4);		
		curr+=16;
		printf("curr inode %u\n",my_dentry[i].inode);
	}
}

int32_t file_close(void){
	return 0;
}

int32_t write_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	return -1;
}

int32_t dir_open(void){
	return 0;
}

int32_t dir_close(void){
	return 0;
}

int32_t dir_write(void){
	return -1;
}
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	if(strlen((int8_t*)fname)>32 || strlen((int8_t*)fname)<1)
		return -1;
	
	int i;
	for(i=0;i<my_boot_block.num_dentries;i++){
		if((uint8_t*)((my_dentry[i]).file_name)==fname){
			dentry->file_type=my_dentry[i].file_type;
			dentry->inode=my_dentry[i].inode;
			return 0;
		}			
	}
	return -1;
	
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	if(index<0 || index>my_boot_block.num_inodes)
		return -1;
	strcpy((int8_t*)dentry->file_name,(int8_t*)(my_dentry[index].file_name));
	dentry->file_type=my_dentry[index].file_type;
	return 0;			
	
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	if(inode<0 || inode>my_boot_block.num_inodes)
		return -1;
	
	uint32_t* curr_inode=inodes+inode*BLOCK_SIZE/BYTE;
	uint8_t curr_length;
	memcpy(&curr_length,curr_inode,1);//length takes one byte
	//unsigned char curr_length=curr_inode->length;
	
	if(offset>curr_length)
		return -1;
	uint32_t* curr_data_block;
	int curr_data_idx;
	int count=length,copied=0,i=0;
	while(count>0){
		memcpy(curr_inode+offset+i,&curr_data_idx,4);
		curr_data_block=data_blocks+curr_data_idx*BLOCK_ADDR_SIZE;
		if(curr_data_block==NULL)//bad block number
			return -1;
		if(count>=BLOCK_SIZE){	
			memcpy(buf,curr_data_block,BLOCK_SIZE);
			copied+=BLOCK_SIZE;
			count-=BLOCK_SIZE;
		}
		else{
			memcpy(buf,curr_data_block,count);
			copied+=count;
			count-=count;
		}		
		i++;
	}
	return copied;
}


