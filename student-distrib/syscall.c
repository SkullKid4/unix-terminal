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
/*
void none() {};
uint32_t stdin_jump_table[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)keyboard_read,(int32_t)invalid_function};
uint32_t stdout_jump_table[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function,(int32_t)terminal_write};
uint32_t rtc_jump_table[4]={(uint32_t)rtc_open,(uint32_t)do_nothing,(uint32_t)rtc_read,(uint32_t)rtc_write};
uint32_t dir_jump_table[4]={(uint32_t)dir_open,(uint32_t)dir_close,(uint32_t)dir_read,(uint32_t)dir_write};
uint32_t default_fops[4]={(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function,(uint32_t)invalid_function}; 
*/
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

void init_fops_table() {
	fops_table[NOT_SET].read = &invalid_function;
	fops_table[NOT_SET].write = &invalid_function;

	fops_table[STDINFLAG].read = &keyboard_read;
	fops_table[STDINFLAG].write = &keyboard_write;

	fops_table[STDOUTFLAG].read = &terminal_read;
	fops_table[STDOUTFLAG].write = &terminal_write;

	fops_table[RTCFLAG].read = &rtc_read;
	fops_table[RTCFLAG].write = &rtc_write;

	fops_table[FILEFLAG].read = &file_read_setup;
	fops_table[FILEFLAG].write = &invalid_function;

	fops_table[DIRFLAG].read = &dir_readnew;
	fops_table[DIRFLAG].write = &invalid_function;
}

/*
int32_t read(int32_t fd, const void* buf, int32_t nbytes)
  Input: fd - file descriptor, indicates where this is being called from
  		buf - the buffer to read
  		nbytes - number of bytes to read
  Return Value: returns number of bytes read, or -1 on failure
  Function: Read a number of bytes to a buffer, according to the file descriptor
*/


int32_t read(int32_t fd, void* buf, int32_t nbytes){
	//printf("%d\n", current_pcb->FDs_array[fd].flags);
	if(nbytes < 0 || buf == NULL || fd > MAX_FILE-1 || fd < 0) return -1;	//fail if fd is out of range of 0-7
	return fops_table[current_pcb->FDs_array[fd].flags].read(fd,buf,nbytes);
	
	/*
	int32_t read_bytes;
	if (fd == STDIN)
		return keyboard_read(0, buf, nbytes);

	if (fd == STDOUT)
		return -1;//return terminal_read(buf, nbytes);

	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if (curr_pcb->FDs_array[fd].flags == NOT_SET)
		return -1;
	if (curr_pcb->FDs_array[fd].flags == RTCFLAG)
		//rtc_write(buf, nbytes);
		return rtc_read();
	if (curr_pcb->FDs_array[fd].flags == DIRFLAG)
		return dir_readnew(buf);//dir_read(curr_pcb->file_name[fd]);//<--need buf for dir_read?
	if (curr_pcb->FDs_array[fd].flags == FILEFLAG) {
		read_bytes = file_read((uint8_t*)curr_pcb->file_name[fd], curr_pcb->FDs_array[fd].file_position, (uint8_t*)buf, nbytes);
		curr_pcb->FDs_array[fd].file_position += read_bytes;
		return read_bytes;
	}

	return -1;*/
			
	//return fops_table[curr_pcb->FDs_array[fd].flags]->read(fd,buf,nbytes);
}
/*
int32_t file_read_setup(int32_t fd, const void* buf, int32_t nbytes)
  Input: fd - file descriptor, indicates where this is being called from
  		buf - the buffer to read
  		nbytes - number of bytes to read
		Return Value: returns number of bytes read, or -1 on failure
  Function: helper function for file_read in order to achieve one-line argument for read, 
			Read a number of bytes to a buffer, according to the file descriptor
*/
int32_t file_read_setup(int32_t fd, void* buf, int32_t nbytes) {
	int32_t read_bytes;
	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	read_bytes = file_read((uint8_t*)curr_pcb->file_name[fd], curr_pcb->FDs_array[fd].file_position, (uint8_t*)buf, nbytes);
	curr_pcb->FDs_array[fd].file_position += read_bytes;
	return read_bytes;
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
	if(nbytes < 0 || buf == NULL || fd < 0 || fd > MAX_FILE-1) return -1;	//fail if fd is out of range of 0-7
	return fops_table[current_pcb->FDs_array[fd].flags].write(fd,buf,nbytes);

	/*if (fd == STDOUT)
		return terminal_write(1, buf, nbytes);

	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if (curr_pcb->FDs_array[fd].flags == NOT_SET)
		return -1;
	if (curr_pcb->FDs_array[fd].flags == RTCFLAG)
		return rtc_write(buf, nbytes);

	return -1;*/

	//return fops_table[curr_pcb->FDs_array[fd].flags]->write(fd,buf,nbytes);
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
	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if(strncmp((int8_t*)filename,"stdin",strlen("stdin"))==0){
		//curr_pcb->FDs_array[0].jump_table_pointer=stdin_jump_table;
		curr_pcb->FDs_array[STDINFLAG].flags = STDINFLAG;
		return 0;
	}
	if(strncmp((int8_t*)filename,"stdout",strlen("stdout"))==0){
		//curr_pcb->FDs_array[1].jump_table_pointer=stdout_jump_table;
		curr_pcb->FDs_array[STDOUTFLAG].flags = STDOUTFLAG;
		return 0;
	}
	if(read_dentry_by_name(filename,&temp_dentry)!=0)
		return -1;
	//get the first open entry in the fd array, and set it with the appropriate info depending on type
	for(i=2;i<8;i++){
		if(curr_pcb->FDs_array[i].flags == NOT_SET){
			if (temp_dentry.file_type == RTCTYPE) {
				//curr_pcb->FDs_array[i].jump_table_pointer=rtc_jump_table;
				curr_pcb->FDs_array[i].flags = RTCFLAG;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				return i;
			}
			else if (temp_dentry.file_type == DIRTYPE) {
				//curr_pcb->FDs_array[i].jump_table_pointer=dir_jump_table;	
				curr_pcb->FDs_array[i].inode=temp_dentry.inode;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				curr_pcb->FDs_array[i].flags=DIRFLAG;
				strcpy((int8_t*)curr_pcb->file_name[i], (int8_t*)filename);
				return i;
			}
			else if (temp_dentry.file_type == FILETYPE) {
				//curr_pcb->FDs_array[i].jump_table_pointer=dir_jump_table;				
				curr_pcb->FDs_array[i].inode=temp_dentry.inode;
				curr_pcb->FDs_array[i].file_position=FILE_START;
				curr_pcb->FDs_array[i].flags=FILEFLAG;
				strcpy((int8_t*)curr_pcb->file_name[i], (int8_t*)filename);
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
	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if(fd==STDIN || fd==STDOUT || fd >MAX_FILE-1 || fd < 0) //fail if fd is out of range
		return -1;
	if(curr_pcb->FDs_array[fd].flags==NOT_SET)
		return -1;
		//curr_pcb->FDs_array[fd].jump_table_pointer=default_fops;
	curr_pcb->FDs_array[fd].inode=0;
	curr_pcb->FDs_array[fd].file_position=0;
	curr_pcb->FDs_array[fd].flags=NOT_SET;
	return 0;		

	//return fops_
		
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


/*
int32_t halt
  Input: status - 8 bit halt status
  Return Value: 32 bit halt status
  Function: halts a process, returning control to the parent process
  			if trying to halt shell (only if process 0), restarts the shell
*/
int32_t halt(uint8_t status){
	//restore parent data
	end_process(curr_process);
	//remove_task(curr_process);

	pcb_t* child_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if(child_pcb->PPID == -1){
		terminals[get_cur_term()].active = 0;
		execute((uint8_t*)("shell\0"));
	}
	//get pcb pointers
	uint32_t cur_ppid = ((pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (curr_process + 1))))->PPID;
	pcb_t* cur_pcb = (pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (cur_ppid + 1)));
	//set tss
	tss.esp0 = PHYS_FILE_START - EIGHT_KB * (cur_ppid) - 4;
	curr_process = cur_ppid;
	set_curr_process(curr_process);

	//retore parent paging
	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + (PHYS_FILE_OFFSET * cur_ppid));

	//close relevent FD's
	int i;
	fds_t* cur_pcb_fd = cur_pcb->FDs_array;

	for(i = 2; i < 8; i++){
		if(cur_pcb_fd[i].flags != 0){
			close(i);
		}
	}

	//jump to execute return
	//first restore parent esp and ebp, and get return value
    asm volatile(
				 ""
                 "mov %0, %%eax;"
                 "mov %1, %%esp;"
                 "mov %2, %%ebp;"
                 "leave;"
                 "ret;"
                 //"jmp HALTED;"
                 :                      /* no outputs */
                 :"r"((uint32_t)status), "r"(child_pcb->ESP0), "r"(child_pcb->EBP0)   /* inputs */
                 :"%eax"                 /* clobbered registers */
                 );

	return 0;
}

/*
int32_t halt_from_exc
  Input: none
  Return Value: 256 to indicate halt from exception
  Function: halts a process, returning control to the parent process
  			if trying to halt shell (only if process 0), restarts the shell
*/
int32_t halt_from_exc(){
	//retore parent data
	end_process(curr_process);
	pcb_t* child_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	if(child_pcb->PPID == -1){
		terminals[get_cur_term()].active = 0;
		execute((uint8_t*)("shell\0"));
	}
	//get pcb pointers
	uint32_t cur_ppid = ((pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (curr_process + 1))))->PPID;
	pcb_t* cur_pcb = (pcb_t*)(PHYS_FILE_START - (EIGHT_KB * (cur_ppid + 1)));
	//set tss
	tss.esp0 = PHYS_FILE_START - EIGHT_KB * (cur_ppid) - 4;
	curr_process = cur_ppid;
	set_curr_process(curr_process);

	//restore parent paging
	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + (PHYS_FILE_OFFSET * cur_ppid));

	//close relevent FD's
	int i;
	fds_t* cur_pcb_fd = cur_pcb->FDs_array;

	for(i = 2; i < MAX_FILE; i++){
		if(cur_pcb_fd[i].flags != 0){
			close(i);
		}
	}

	//jump to execute return
	//first restore parent esp and ebp, and get return value (256 because of exception)
    asm volatile(
				 ""
                 "mov %0, %%eax;"
                 "mov %1, %%esp;"
                 "mov %2, %%ebp;"
                 "leave;"
                 "ret;"
                 //"jmp HALTED;"
                 :                      /* no outputs */
                 :"r"(256), "r"(child_pcb->ESP0), "r"(child_pcb->EBP0)   /* inputs */ //return 256 when returning execute because of exception
                 :"%eax"                 /* clobbered registers */
                 );

	return 0;
}



/*
int32_t execute
  Input: command - the process to execute
  Return Value: -1 if cannot execute
  				0-255 if halted by "halt" function
  				256 if halted by exception
  Function: executes a process
*/

 int32_t execute (const uint8_t* command) {


 	uint8_t buf[BUF_SIZE];
 	uint8_t com[BUF_SIZE];
 	uint8_t buffer[SMALL_BUF];
 	uint8_t* scan;
 	strcpy ((int8_t*)buf, (int8_t*)command);
 	uint32_t i = 0;
 	uint32_t file_start;
 	uint8_t arg_start;

 	// parse command
 	for (scan = buf; '\0' != *scan && ' ' != *scan && '\n' != *scan; scan++) {
 		com[i] = command[i];
 		i++;
 	}
 	com[i] = '\0';



	while(command[i] == ' ' ) {
 		i++;
 	}
 	uint32_t new_process = get_process();
 	if (new_process == -1){
 		printf("too many processes\n");
 		return 0;	//fail if we have 6 processes already
 	}
 	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (new_process + 1));
 	current_pcb = curr_pcb;
 	arg_start = i;
 	while(command[i] != '\0'){
 		curr_pcb->args[i-arg_start] = command[i];
 		i++;
 	}
 	curr_pcb->args[i-arg_start] = '\0';
 	
 	//check if file is valid
 	dentry_t valid_file_check;
 	if (read_dentry_by_name((uint8_t*)com, &valid_file_check) != 0){
 		//printf("invalid file");
 		clear_process(new_process);
 		return -1;
 	}
 	//check for valid exe
 	read_data(valid_file_check.inode, 0, buffer, SMALL_BUF);
 	if (buffer[0] != 0x7F || buffer[1] != 0x45 || buffer[2] != 0x4C || buffer[3] != 0x46){ //check first four bytes of file to determine that it is a valid exe
 																						// file The first four bytes must match these exactly, or it's invalid
 		return -1;
 	}
 	// get start address

 	read_data(valid_file_check.inode, 24, buffer, SMALL_BUF); //read bytes 24-27 to get the location of the first line of code to execute
 	file_start = *((uint32_t*)buffer);

 	//initialize pcb
 	if (terminals[get_cur_term()].active == 0) {
 		terminals[get_cur_term()].active = 1;
 		curr_pcb->PPID = -1;
 	}
 	else
 	curr_pcb->PPID = curr_process;
 	
 	curr_pcb->PID = new_process;
 	curr_process = new_process;
 	set_curr_process(curr_process);


 	//initialize fd_array
 	for (i = 0; i < MAX_FILE; i++) {
 		//curr_pcb->FDs_array[i].jump_table_pointer = default_fops;
 		curr_pcb->FDs_array[i].inode = -1;
 		curr_pcb->FDs_array[i].file_position = FILE_START;
 		curr_pcb->FDs_array[i].flags = NOT_SET;
 	}

 	//curr_pcb->FDs_array[0].jump_table_pointer = stdin_jump_table;
 	curr_pcb->FDs_array[0].flags = STDINFLAG;
  	//curr_pcb->FDs_array[0].jump_table_pointer = stdout_jump_table;
 	curr_pcb->FDs_array[1].flags = STDOUTFLAG;

 	


 	// map new page
 	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * new_process);

 	// write file into memory
 	read_data(valid_file_check.inode, 0, (uint8_t*)VIRTUAL_FILE_START, READ_WHOLE_FILE); //100000 is just a large number to read the whole file

 	//set tss
 	tss.ss0 = KERNEL_DS;
 	tss.esp0 = PHYS_FILE_START - EIGHT_KB * (new_process) - 4;
 	//store ebp and esp
 	 	asm volatile("			\n\
				movl %%ebp, %%eax 	\n\
				movl %%esp, %%ebx 	\n\
			"
	:"=a"(curr_pcb->EBP0), "=b"(curr_pcb->ESP0));
	//add_task(curr_pcb->ESP0,curr_process);
	//if(curr_process == 1){
	//	remove_task(0);
	//}
	
	
 	jump_user_space(file_start);
 	// sti();
 	//printf("%d\n", current_pcb->FDs_array[fd].flags);
    return 0;

 }


/*
int32_t getargs
  Input: buf - buffer to write command line args into
  		nbytes - # bytes to write
  Return Value:  0 on success
				-1 on failure
  Function: writes command line arguments into a buffer for userspace
*/
int32_t getargs(uint8_t* buf, int32_t nbytes){
	if (buf == NULL)
		return -1;
	pcb_t* curr_pcb = (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (curr_process + 1));
	strcpy((int8_t*)buf, (int8_t*)curr_pcb->args);
	return 0;
}

/*
int32_t vidmap
  Input: screen_start - pointer to address in userspace to load video memory into
  Return Value: 0 on success
  				-1 on failure
  Function: maps video memory into user space
*/
int32_t vidmap(uint8_t** screen_start){
	//check if screen_start is in the user file.
	if ( (uint32_t)screen_start < VIRTUAL_FILE_PAGE || (uint32_t)screen_start > VIRTUAL_FILE_PAGE + PHYS_FILE_OFFSET)
		return -1;
	map_w_pt(USER_VID_MEM, (uint32_t)get_vid_mem());
	*screen_start = (uint8_t*)USER_VID_MEM;
	return 0;
}

/*
int32_t set_handler
  Input: signum - which signal handler to change
  		handler_address - points to user-level function to run when signal is received
  Return Value: 0 on success
  				-1 on failure
  Function: sets a signal handler to a user-specified funciton
*/
int32_t set_handler(int32_t signum, void* handler_address){
	return -1;
}

/*
int32_t sigreturn
  Input: none
  Return Value: 0 on success
  				-1 on failure
  Function: copies hardware context from user-level stack to processor
*/
int32_t sigreturn(void){
	return -1;
}


/*
int32_t get_process
  Input: none
  Return Value: the index of the new process to run, -1 if max # processes already running
  Function: used to parse running processes and find a space for the process to run, asserts a maximum number of processes
*/
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

/*
void end_process
  Input: proc_num - the process to end
  Return Value: none
  Function: sets the entry in process_array for the process to 0, indicating that another process may be run in its place
*/
void end_process(int32_t proc_num) {
	process_array[proc_num] = 0;
}
/*
int32_t invalid_function(int32_t fd, const void* buf, int32_t nbytes)
  Input: fd,buf,nbytes-be consistent with read and write syscalls arguments
  Return Value: -1
  Function: place holders for invalid syscalls in file structs
*/

int32_t invalid_function(int32_t fd, void* buf, int32_t nbytes) {
	return -1;
}


/*int32_t do_nothing(){
	return 0;
}*/

/*
void clear_process
  Input: none
  Return Value: none
  Function: sets all entries in process_array to 0, used before executing shell in kernel.c
*/
void clear_process(){
	int i;
	for (i = 0; i < NUM_PROCESSES; i++){ // We can have a maximum of 6 processes. set them all to unused.
		process_array[i] = 0;
	}
}

uint32_t get_current_process() {
	return curr_process;
}

pcb_t* get_pcb_pointer(int process){
	return (pcb_t*)(PHYS_FILE_START - EIGHT_KB * (process + 1));

}

void set_process_sys(int process) {
	curr_process = process;
}
