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
		if(strncmp((int8_t*)(my_dentry[i].file_name),(int8_t*)fname,strlen((int8_t*)fname))==0){
			memset(dentry->file_name,' ',MAX_FILE_CHAR+1);
			strcpy((int8_t*)(dentry->file_name),(int8_t*)fname);
			dentry->file_type=my_dentry[i].file_type;
			dentry->inode=my_dentry[i].inode;
			return 0;
		}			
	}
	return -1;
	
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	if(index<0 || index>=my_boot_block.num_dentries)
		return -1;
	strcpy((int8_t*)dentry->file_name,(int8_t*)(my_dentry[index].file_name));
	dentry->file_type=my_dentry[index].file_type;
	dentry->inode=my_dentry[index].inode;
	return 0;			
	
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	if(inode<0 || inode>=my_boot_block.num_inodes)
		return -1;
	
	uint32_t* curr_inode=inodes+inode*BLOCK_ADDR_SIZE;
	int file_reached_end=0;
	uint32_t curr_length;
	memcpy(&curr_length,curr_inode,4);//length takes one byte
	if(offset>curr_length)
		return -1;
	
	uint8_t* curr_data_block;
	int curr_data_idx,curr_block_byte;
	int count=0;//count the location within the file
	int copied=0;
	int i=1;
	
	if(offset>BLOCK_SIZE)
		i+=offset/BLOCK_SIZE;
	memcpy(&curr_data_idx,curr_inode+i,4);
	curr_block_byte=offset%BLOCK_SIZE;
	count+=offset;
	while(copied<length || !file_reached_end){
		if(curr_block_byte>=BLOCK_SIZE){
			curr_block_byte=0;
			i++;
			memcpy(&curr_data_idx,curr_inode+i,4);
		}
		if(curr_data_idx>=my_boot_block.num_data_blocks)//bad block number
			return -1;
		curr_data_block=(uint8_t*)(data_blocks+curr_data_idx*BLOCK_ADDR_SIZE);
		
		//copy one byte
		memcpy(buf+copied,curr_data_block+curr_block_byte,1);
		copied++;
		count++;
		curr_block_byte++;
		if(count>=curr_length)	//reached the end of the file
			file_reached_end=1;
	}
	if(file_reached_end)
		return 0;
	return copied;
}


