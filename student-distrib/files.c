#include "files.h"
#include "lib.h"
#include "keyboard.h"
#include "terminal.h"

/*
void file_open(const uint8_t* filename)
  Input: add_start -- the starting address of file system
  Return Value: none
  Function: set up the file system. Copy necessary statistics to defined 
			struct in file.h and critical addresses
*/
uint32_t dir_read_idx=0;

void file_open(uint32_t* add_start){
	my_file_sys=add_start;
	memcpy((void*)(&my_boot_block),(void*)my_file_sys,STAT_SIZE);
	inodes=my_file_sys+BLOCK_ADDR_SIZE;
	data_blocks=my_file_sys+BLOCK_ADDR_SIZE*(1+my_boot_block.num_inodes);
	
	int i;
	uint32_t* curr=my_file_sys+STAT_ADDR_SIZE;//calculate starting address of the statistics
	for(i=0;i<my_boot_block.num_dentries;i++){
		memcpy(&(my_dentry[i].file_name),curr,MAX_FILE_CHAR);//copy each dentry info to dentry struct
		memcpy(&(my_dentry[i].file_type),curr+POS_FILE_TYPE,4);
		memcpy(&(my_dentry[i].inode),curr+POS_FILE_INODE,4);	
		curr+=STAT_ADDR_SIZE;
	}
}
/*
int32_t file_close
  Input: none
  Return Value: 0-- always success
  Function: close file system
*/

int32_t file_close(void){
	return 0;
}
/*
int32_t file_write
  Input: fname -- the file that the function wants to write to
		 offset -- offset from the start position of the buffer
		 buf -- data that to be written to inode
		 length -- data length to be written in bytes 
  Return Value: -1, always fail
  Function: the file system is read-only file system, so always fail for
			write function
*/
int32_t file_write(const uint8_t* fname, uint32_t offset, uint8_t* buf,uint32_t length){
	return -1;
}

/*
int32_t file_read
  Input: fname -- the file that the function wants to read from
		 offset -- offset from the start position of the buffer
		 buf -- data that to be written to 
		 length -- data length to be written in bytes 
  Return Value: number of bytes successfully read or 0 indicated the file reaches to the end
				-1 - fail
  Function: the file system is read-only file system, so always fail for
			write function
*/
int32_t file_read(const uint8_t* fname, uint32_t offset, uint8_t* buf,uint32_t length){
	dentry_t curr_dentry;
	if(read_dentry_by_name(fname,&curr_dentry)==0){
		return read_data(curr_dentry.inode,0,buf,length);
	}
	return -1;
}


/*
int32_t write_data
  Input: inode -- the inode that the function wants to write to
		 offset -- offset from the start position of the buffer
		 buf -- data that to be written to inode
		 length -- data lenth to be written in bytes 
  Return Value: -1, always fail
  Function: the file system is read-only file system, so always fail for
			write function
*/
int32_t write_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	return -1;
}

/*
int32_t dir_open
  Input: none
  Return Value: 0-- always success
  Function: open a directory, nothing is implemented right now
*/

int32_t dir_open(void){
	return 0;
}

/*
int32_t dir_close
  Input: none
  Return Value: 0-- always success
  Function: close a directory, nothing is implemented right now
*/

int32_t dir_close(void){
	return 0;
}
/*
int32_t dir_write
  Input: none
  Return Value: -1 -- always fail
  Function: the file system is read-only file system, so always fail for
			write function
*/

int32_t dir_write(void){
	return -1;
}
/*
int32_t dir_read
  Input: none
  Return Value: 0 -- always success
  Function: Directory read gives all necessary information about files stored in the system
*/

int32_t dir_read(int8_t* buf){
	int i;
	uint32_t file_size;
	int8_t* temp;
	uint32_t curr_length=0;
	if(dir_read_idx>=my_boot_block.num_dentries)
		dir_read_idx=0;
	memcpy(&file_size,inodes+(my_dentry[dir_read_idx].inode)*BLOCK_ADDR_SIZE,4);
	strcpy(buf,"file_name:");
	curr_length+=strlen("file_name:");
	strncpy(buf+curr_length,(int8_t*)my_dentry[dir_read_idx].file_name,strlen((int8_t*)my_dentry[i].file_name));
	curr_length+=strlen((int8_t*)my_dentry[dir_read_idx].file_name);
	strncpy(buf+curr_length,"file_type:",strlen("file_type:"));
	curr_length+=strlen("file_type:");
	itoa(my_dentry[dir_read_idx].file_type, temp, 10);
	strncpy(buf+curr_length,temp,strlen(temp));
	curr_length+=strlen(temp);
	strncpy(buf+curr_length,"file_size:",strlen("file_size:"));
	curr_length+=strlen("file_size:");
	itoa(file_size, temp, 10);
	strncpy(buf+curr_length,temp,strlen(temp));
	curr_length+=strlen(temp);
	dir_read_idx++;
	/*write(STDOUT,"file_name:",strlen("file_name:"));
			write(STDOUT,my_dentry[i].file_name,strlen((int8_t*)my_dentry[i].file_name));
			write(STDOUT,"         ",strlen("          "));
			write(STDOUT,"file_type:",strlen("file_type:"));
			itoa(my_dentry[i].file_type, one_line_buf, 10);//10 for decimal system 
			write(STDOUT,one_line_buf,strlen(one_line_buf));
			write(STDOUT,"      ",strlen("      "));
			strcpy(one_line_buf,"");
			write(STDOUT,"file_size:",strlen("file_size:"));
			itoa(file_size, one_line_buf, 10);//10 for decimal system
			write(STDOUT,one_line_buf,strlen(one_line_buf));
			putc('\n');	*/
	return 0;
}

/*
int32_t read_dentry_by_name
  Input: fname -- file name to be read from
		 dentry -- struct that will be copied to if fname is successfully found
  Return Value: 0 -- success
				-1 -- fail
  Function: Given a file name, if it can be found in the file system, then
			copy the directory entry information to given pointer dentry_t*
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	//check if the length of file name is within the maximum char 
	if(strlen((int8_t*)fname)>32 || strlen((int8_t*)fname)<1)
		return -1;
	
	int i;
	//search for the given file name, and copy necessary coresponding info of the file to given pointer dentry_t*
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

/*
int32_t read_dentry_by_index
  Input: index -- file index to be read from
		 dentry -- struct that will be copied to if fname is successfully found
  Return Value: 0 -- success
				-1 -- fail
  Function: Given a file index, if it can be found in the file system, then
			copy the directory entry information to given pointer dentry_t*
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
	//check if the give index is within the maximum inode 
	if(index<0 || index>=my_boot_block.num_dentries)
		return -1;
	//copy necessary coresponding info of the file to given pointer dentry_t*	
	strcpy((int8_t*)dentry->file_name,(int8_t*)(my_dentry[index].file_name));
	dentry->file_type=my_dentry[index].file_type;
	dentry->inode=my_dentry[index].inode;
	return 0;			
	
}

/*
int32_t read_data
  Input: inode -- the inode that the function wants to read from
		 offset -- offset from the start position of the buffer
		 buf -- data that to be read from inode
		 length -- data lenth to be written in bytes 
  Return Value: 0 -- success -1 -- fail
  Function: Given a inode and offset, read as many bytes as possible from the inode
			to given buffer
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf,uint32_t length){
	//check if inode is within the maximum number of inodes in the block
	if(inode<0 || inode>=my_boot_block.num_inodes)
		return -1;
	
	uint32_t* curr_inode=inodes+inode*BLOCK_ADDR_SIZE;//starting index of given inode
	int file_reached_end=0;//flag to indicate whether the file reaches the end or not 
	uint32_t curr_length;
	memcpy(&curr_length,curr_inode,4);//length takes one byte
	//check if offset is longer than the file length
	if(offset>curr_length)
		return -1;
	
	uint8_t* curr_data_block;//starting address of current data block to be read from
	int curr_data_idx,curr_block_byte;
	int count=0;//count the location within the file
	int copied=0;
	int i=1;
	
	//adjust the beginning position for read function
	if(offset>BLOCK_SIZE)
		i+=offset/BLOCK_SIZE;
	memcpy(&curr_data_idx,curr_inode+i,4);
	curr_block_byte=offset%BLOCK_SIZE;
	count+=offset;
	
	while(copied<length || !file_reached_end){
		if(curr_block_byte>=BLOCK_SIZE){//check if last block has reached the end
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


