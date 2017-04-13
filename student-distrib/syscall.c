#include "syscall.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "files.h"
#include "rtc.h"
#include "terminal.h"
#include "paging.h"
#include "x86_desc.h"
#include "syscall_link.h"

void none() {};
uint32_t stdin_jump_table[4]={(uint32_t)none,(uint32_t)keyboard_close,(uint32_t)keyboard_read,(int32_t)keyboard_write};
uint32_t stdout_jump_table[4]={(uint32_t)none,(uint32_t)terminal_close,(uint32_t)terminal_read,(int32_t)terminal_write};
uint32_t files_jump_table[4]={(uint32_t)file_open,(uint32_t)file_close,(uint32_t)file_read,(int32_t)file_write};
uint32_t rtc_jump_table[4]={(uint32_t)rtc_open,(uint32_t)rtc_close,(uint32_t)rtc_read,(uint32_t)rtc_write};
uint32_t dir_jump_table[4]={(uint32_t)dir_open,(uint32_t)dir_close,(uint32_t)dir_read,(uint32_t)dir_write};

fops_t std_in = {keyboard_read, invalid_function, invalid_function, invalid_function};
fops_t std_out = {invalid_function, keyboard_write, invalid_function, invalid_function};
fops_t rtc = {rtc_read, rtc_write, rtc_open, rtc_close};
fops_t file = {file_read, file_write, file_open, file_close};
fops_t direct = {dir_read, dir_write, dir_open, dir_close};
fops_t default_fops = {invalid_function, invalid_function, invalid_function, invalid_function};

/*
void read()
  Input: 
  	fd: the file descriptor, signifies which kind of read should be called
  	buf: the buffer that the read data should be placed in
  	nbytes: the number of bytes that should be read into the buffer
  Return Value: The number of bytes read
  Function: Varies per file descripter. Reading STDIN (the keyboard) reads the last line that was terminated with a new line
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	if(nbytes < 0 || buf == NULL || fd > 7 || fd < 0) return -1;	
	
	switch(fd){
		case STDIN:
			return keyboard_read(buf, nbytes);
			break;

		case STDOUT:
			return terminal_read(buf, nbytes);
			break;

		default:
			return file_read(file_name[fd], 0, buf, nbytes);
			break
	};
	return -1;
}

/*
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
  Input: fd - file descriptor, indicates where this is being called from
  		buf - the buffer to write
  		nbytes - number of bytes to write
  Return Value: returns number of bytes written, or -1 on failure
  Function: Writes a number of bytes from a buffer, according to the file descriptor
*/
int32_t write(int32_t fd, void* buf, int32_t nbytes){
	if(nbytes < 0 || buf == NULL || fd > 7 || fd < 0) return -1;	

	switch(fd){
		case STDIN:
			return keyboard_write(buf, nbytes);
			break;

		case STDOUT:
			return terminal_write(buf, nbytes);
			break;
		default:
			return file_read(file_name[fd], 0, buf, nbytes);
			break
	};

	return -1;
}

/*
int32_t open(const uint8_t* filename)
  Input: filename - the name of the file to open
  Return Value: returns 0 on sucsess, or -1 on failure
  Function: opens a file
*/
int32_t open(const uint8_t* filename){
	dentry_t temp_dentry;
	int i=2;
	if(strncmp((int8_t*)filename,"stdin",strlen("stdin"))==0){
		my_fds[0].jump_table_pointer=*stdin_jump_table;
		my_fds[0].inode=NULL;
		my_fds[0].flags=IN_USE;
		return 0;
	}
	if(strncmp((int8_t*)filename,"stdout",strlen("stdout"))==0){
		my_fds[1].jump_table_pointer=*stdout_jump_table;
		my_fds[1].inode=NULL;
		my_fds[1].flags=IN_USE;
		return 0;
	}
	if(read_dentry_by_name(filename,&temp_dentry)!=0)
		return -1;
	
	for(i=2;i<8;i++){
		if(my_fds[i].flags!=IN_USE){
			if(strncmp((int8_t*)filename,"rtc",strlen("rtc"))==0)
				my_fds[i].jump_table_pointer=*rtc_jump_table;
			else
				my_fds[i].jump_table_pointer=*files_jump_table;				
			my_fds[i].inode=temp_dentry.inode;
			my_fds[i].file_position=0;
			my_fds[i].flags=IN_USE;
			return 0;
		}				
	}
	if(i>=8)
		return -1;//file descriptors are full;	
	return 0;
}

/*
int32_t close(const uint8_t* filename)
  Input: filename - the name of the file to close
  Return Value: returns 0 on sucsess, or -1 on failure
  Function: closes a file
*/
int32_t close(int32_t fd){
	/*if(fd == STDIN){
		disable_irq(KEYBOARD_IRQ);
	}else if(fd == STDOUT){
		return 0;
	}
	return -1;*/
	/*switch(fd) {
		case STDIN:
			return keyboard_close();
			break;

		case STDOUT:
			return terminal_close();
			break;
	}*/

	if(fd==0 || fd==1 || fd >7)
		return -1;
	if(my_fds[fd].flags==NOT_IN_USE)
		return -1;
	if(my_fds[fd].flags==IN_USE){
	/*	my_fds[fd].jump_table_pointer==NULL;
		my_fds[fd].inode==0;
		my_fds[fd].file_position==0;
		my_fds[fd].flags==NOT_IN_USE;
		return 0;		*/
	}
	return -1;
		
}




/*
syscall tasks

1. move functions in syscall into respective files. So system call can just call them resectivley.
2. check user privelege of interup, kill it if user generates an execption
DONE 		3. Set idt entery x80 to our wrapper code
4. rewrite exsisting code to call the general system call
5. write system calls for the functions defined in the documentation
6. write assemby linke (copy from syscall/ emulate.c)




THINGS TO CORRECT
DONE 	1. Clear the buffer when enter is pressed (dont forget to get points back)


Update keyboard.c for writes to STDOUT to go directly to specific write function
*/


int32_t halt(uint8_t status){
	//retore paret data
	uint32_t cur_ppid = (pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (curr_process + 1)))->PPID;
	pcb_t* cur_pcb = (pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (curr_ppid + 1)));
	tss.esp0 = cur_pcb->ESP0;
	curr_process = cur_ppid;

	//retore parent paging
	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + (PHYS_FILE_OFFSET * cur_ppid));

	//close relevent FD's
	int i;
	fds_t* cur_pcb_fd = cur_pcb->FDs_array;

	for(i = 2; i < 8; i++){
		if(cur_pcb_fd[i].flags != 0){
			file_close(i);
		}
	}

	//jump to execute return
	asm volatile ("              \
        JMP HALTED              ;\
	")

	return 0;
}

 int32_t execute (const uint8_t* command) {


 	uint8_t buf[256];
 	uint8_t com[128];
 	uint8_t buffer[4];
 	char* args[128];
 	uint8_t* scan;
 	strcpy ((int8_t*)buf, (int8_t*)command);
 	uint32_t i = 0;
 	uint32_t file_start;

 	for (scan = buf; '\0' != *scan && ' ' != *scan && '\n' != *scan; scan++) {
 		com[i] = command[i];
 		i++;
 	}
 	strcpy((int8_t*)args, (int8_t*)buf);
 	/*
 	com[i+1] = '\0';
 	args[0] = (char*)buf;
 	n_arg = 1;
 	if ('\0' != *scan) {
 		*scan++ = '\0';
 		// parse arguments
	 	while (1) {
	 		while (' ' == *scan) scan++;
	 		if ('\0' == *scan || '\n' == *scan) {
	 			*scan = '\0';
	 			break;
	 		}
	 		args[n_arg++] = (char*)scan;
	 		while ('\0' != *scan && ' ' != *scan && '\n' != *scan) scan++;
	 		if ('\0' != *scan)
	 			*scan++ = '\0';
 		}
 	}
 	args[n_arg] = NULL;
 	if (0 == for)
 	*/
 	
 	//check if file is valid
 	dentry_t valid_file_check;
 	if (read_dentry_by_name((uint8_t*)com, &valid_file_check) != 0){
 		//printf("invalid file");
 		return -1;
 	}
 	//check for valid exe
 	read_data(valid_file_check.inode, 0, buffer, 4);
 	if (buffer[0] != 0x7F || buffer[1] != 0x45 || buffer[2] != 0x4C || buffer[3] != 0x46){
 		return -1;
 	}
 	// get start address

 	read_data(valid_file_check.inode, 24, buffer, 4);
 	file_start = *((uint32_t*)buffer);

 	uint32_t new_process = get_process();
 	if (new_process == -1)
 		return -1	//fail if we have 6 processes already
 	pcb_t* curr_pcb = PHYS_FILE_START - EIGHT_KB * (new_process + 1);
 	curr_pcb->PPID = curr_process;
 	curr_pcb->PID = new_process;
 	curr_pcb->ESP0 = PHYS_FILE_START - EIGHT_KB * (process_num) - 4
 	curr_process = new_process;

 	//initialize fd_array
 	for (i = 0; i < MAX_FILES; i++) {
 		curr_pcb->fds[i].fops = default_fops;
 	}

 	// map new page
 	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * process_num);

 	// write file into memory
 	read_data(valid_file_check.inode, 0, (uint8_t*)VIRTUAL_FILE_START, 100000);

 	tss.ss0 = KERNEL_DS;
 	tss.esp0 = curr_pcb->ESP0;
 	jump_user_space(file_start);
 	// sti();

    return 0;

 }


int32_t getargs(uint8_t* buf, int32_t nbytes){
	return 0;
}

int32_t vidmap(uint8_t** sreen_start){
	return 0;
}

int32_t set_handler(int32_t signum, void* handler_address){
	return 0;
}

int32_t sigreturn(void){
	return 0;
}


