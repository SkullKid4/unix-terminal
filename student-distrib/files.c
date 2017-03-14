#include "files.h"
#include "lib.h"

void fs_open(void){
	
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	if(strlen((int8_t*)fname)>32 || strlen((int8_t*)fname)<1)
		return -1;
	
	int i;
	boot_block_t* boot_block_prt=&(my_file_sys.my_boot_block);
	for(i=0;i<boot_block_prt->num_dentries;i++){
		if((uint8_t*)((boot_block_prt->my_dentry[i]).file_name)==fname){
			dentry->file_type=(boot_block_prt->my_dentry[i]).file_type;
			dentry->inode=(boot_block_prt->my_dentry[i]).inode;
			return 0;
		}			
	}
	return -1;
	
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	boot_block_t* boot_block_prt=&(my_file_sys.my_boot_block);
	if(index<0 || index>boot_block_prt->num_inodes)
		return -1;
	strcpy((int8_t*)dentry->file_name,(int8_t*)(boot_block_prt->my_dentry[index]).file_name);
	dentry->file_type=(boot_block_prt->my_dentry[index]).file_type;
	return 0;			
	
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	boot_block_t* boot_block_prt=&(my_file_sys.my_boot_block);
	
	if(inode<0 || inode>boot_block_prt->num_inodes)
		return -1;
	inode_t* curr_inode=(inode_t*)(my_file_sys.inodes+inode*BLOCK_SIZE);
	unsigned char curr_length=curr_inode->length;
	
	if(offset>curr_length)
		return -1;
	data_block_t* curr_data_block;
	int count=length,copied=0,i=0;
	while(count>0){
		curr_data_block=(data_block_t*)(my_file_sys.data_blocks+(curr_inode->data_block_index[offset+i])*BLOCK_SIZE);
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


