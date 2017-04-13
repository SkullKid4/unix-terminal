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
uint32_t stdin_jump_table[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)keyboard_read,(int32_t)invalid_function};
uint32_t stdout_jump_table[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function,(int32_t)terminal_write};
uint32_t rtc_jump_table[4]={(uint32_t)rtc_open,(uint32_t)do_nothing,(uint32_t)rtc_read,(uint32_t)rtc_write};
uint32_t dir_jump_table[4]={(uint32_t)dir_open,(uint32_t)dir_close,(uint32_t)dir_read,(uint32_t)dir_write};
uint32_t default_fops[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function}; 

/*
fops_t std_in = {keyboard_read, invalid_function, invalid_function, invalid_function};
fops_t std_out = {invalid_function, terminal_write, invalid_function, invalid_function};
fops_t rtc = {rtc_read, rtc_write, rtc_open, rtc_close};
fops_t file = {file_read, file_write, file_open, file_close};
fops_t direct = {dir_read, dir_write, dir_open, dir_close};
fops_t default_fops = {invalid_function, invalid_function, invalid_function, invalid_function};
*/

/*
void read()
  Input: 
  	fd: the file descriptor, signifies which kind of read should be called
  	buf: the buffer that the read data should be placed in
  	nbytes: the number of bytes that should be read into the buffer
  Return Value: The number of bytes read
  Function: Varies per file descripter. Reading STDIN (the keyboard) reads the last line that was terminated with a new line
*/
// look at later -Sam
int32_t read(int32_t fd, void* buf, int32_t nbytes){
	int32_t read_bytes;
	if(nbytes < 0 || buf == NULL || fd > 7 || fd < 0) return -1;	
	
	if (fd == STDIN)
		return keyboard_read(buf, nbytes);

	if (fd == STDOUT)
		return terminal_read(buf, nbytes);

	pcb_t* curr_pcb = PHYS_FILE_START - EIGHT_KB * (new_process + 1);
	if (curr_pcb.FDs_array[fd].flags == NOT_SET)
		return -1;
	if (curr_pcb->FDs_array[fd].flags == RTCFLAG)
		rtc_write(buf, nbytes);
	if (curr_pcb->FDs_array[fd].flags == DIRFLAG)
			return dir_read(curr_pcb->file_name[fd]);
	if (curr_pcb->FDs_array[fd].flags == FILEFLAG) {
		read_bytes = file_read(curr_pcb->file_name[fd], curr_pcb->FDs_array[fd].file_position, buf, nbytes);
		curr_pcb->FDs_array[fd].file_position += read_bytes;
		return read_bytes;
	}
			
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
	if(nbytes < 0 || buf == NULL || fd < 0 || fd > 7) return -1;	

	if (fd == STDOUT)
		return terminal_write(buf, nbytes);

	pcb_t* curr_pcb = PHYS_FILE_START - EIGHT_KB * (new_process + 1);
	if (curr_pcb.FDs_array[fd].flags == NOT_SET)
		return -1;
	if (curr_pcb.FDs_array[fd].flags == RTCFLAG)
		return rtc_write(buf, nbytes);
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
	pcb_t* curr_pcb = PHYS_FILE_START - EIGHT_KB * (new_process + 1);
	if(strncmp((int8_t*)filename,"stdin",strlen("stdin"))==0){
		curr_pcb->FDs_array[0].jump_table_pointer=stdin_jump_table;
		return 0;
	}
	if(strncmp((int8_t*)filename,"stdout",strlen("stdout"))==0){
		curr_pcb->FDs_array[1].jump_table_pointer=stdout_jump_table;
		return 0;
	}
	if(read_dentry_by_name(filename,&temp_dentry)!=0)
		return -1;
	
	for(i=2;i<8;i++){
		if(my_fds[i].flags!=IN_USE){
			if (temp_dentry.file_type == RTCTYPE) {
				curr_pcb->FDs_array[i].jump_table_pointer=rtc_jump_table;
				curr_pcb->FDs_array[i].flags = RTCFLAG;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				return i;
			}
			else if (temp_dentry.file_type == DIRTYPE) {
				curr_pcb->FDs_array[i].jump_table_pointer=dir_jump_table;				
				curr_pcb->FDs_array[i].inode=temp_dentry.inode;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				curr_pcb->FDs_array[i].flags=DIRFLAG;
				strcpy(curr_pcb->file_name[i], filename);
				return i;
			else if (temp_dentry.file_type == FILETYPE) {
				curr_pcb->FDs_array[i].jump_table_pointer=dir_jump_table;				
				curr_pcb->FDs_array[i].inode=temp_dentry.inode;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				curr_pcb->FDs_array[i].flags=DIRFLAG;
				strcpy(curr_pcb->file_name[i], filename);
				return i;
			}
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
	pcb_t* curr_pcb = PHYS_FILE_START - EIGHT_KB * (new_process + 1);
	if(fd==0 || fd==1 || fd >7 || fd < 0)
		return -1;
	if(curr_pcb->FDs_array[fd].flags==NOT_SET)
		return -1;
	if(curr_pcb->FDs_array[fd].flags==IN_USE){
		curr_pcb->FDs_array[fd].jump_table_pointer=default_fops;
		curr_pcb->FDs_array[fd].inode=0;
		curr_pcb->FDs_array[fd].file_position=0;
		curr_pcb->FDs_array[fd]=NOT_SET;
		return 0;		
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
	if(curr_process == 0){
		end_process(0);
		execute((uint8_t*)("shell\0"));
	}

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
 	curr_pcb->ESP0 = PHYS_FILE_START - EIGHT_KB * (process_num) - 4;
 	curr_process = new_process;

 	//initialize fd_array
 	for (i = 0; i < MAX_FILES; i++) {
 		curr_pcb->fds[i].fops = default_fops;
 		curr_pcb->fds[i].inode = -1;
 		curr_pcb->fds[i].file_position = FILE_START;
 		curr_pcb->fds[i].flags = NOT_SET;
 	}

 	process_control_block->fds[0].jump_table_pointer = stdin_jump_table;
 	process_control_block->fds[0].flags = STDINFLAG;
  	process_control_block->fds[0].jump_table_pointer = stdout_jump_table;
 	process_control_block->fds[0].flags = STDOUTFLAG;


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

int32_t get_process() {
	int i;
	for (i = 0; i < NUM_PROCESSES; i++) {
		if (process_array[i] == 0) {
			process_array[i] = 1;
			return i;
		}
	}
	return -1;
}
void end_process(int32_t proc_num) {
	process_array[proc_num] = 0;
}

void invalid_function(){
	return -1;
}

void do_nothing(){
	return 0;
}


