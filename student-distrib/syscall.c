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
void system_call_handler()
  Input: none
  Return Value: none 
  Function: Calls the apropriate system call based in the argumet registers. The call number is found in EAX
*/

/*void DO_CALL(void name, void number, void arg1, void arg2, void arg3) {
	asm volatile ("                    \
.GLOBL " #name "                  ;\
" #name ":                        ;\
        PUSHL %EBX              ;\
  MOVL  $" #number ",%EAX ;\
  MOVL  $" #arg1 ",%EBX      ;\
  MOVL  $" #arg2 ",%ECX     ;\
  MOVL  $" #arg3 ",%EDX     ;\
  INT $0x80             ;\
  POPL  %EBX              ;\
  LEAVE						;\
  RET 						;\
  ")
}*/

/*jump tables for functions using in file descriptors*/
/*open,close,read,write*/
void none() {};
uint32_t stdin_jump_table[4]={(uint32_t)none,(uint32_t)keyboard_close,(uint32_t)keyboard_read,(int32_t)keyboard_write};
uint32_t stdout_jump_table[4]={(uint32_t)none,(uint32_t)terminal_close,(uint32_t)terminal_read,(int32_t)terminal_write};
uint32_t files_jump_table[4]={(uint32_t)file_open,(uint32_t)file_close,(uint32_t)file_read,(int32_t)file_write};
uint32_t rtc_jump_table[4]={(uint32_t)rtc_open,(uint32_t)rtc_close,(uint32_t)rtc_read,(uint32_t)rtc_write};
uint32_t dir_jump_table[4]={(uint32_t)dir_open,(uint32_t)dir_close,(uint32_t)dir_read,(uint32_t)dir_write};


//uint32_t kernel_bottom=0x800000;


void system_call_handler()
{

	register int call_number asm("eax");		//For system calls, the args are passed through registers.
	register int arg1 asm("ebx");				//See Appendix B in assignment for specifics
	register int arg2 asm("ecx");
	register int arg3 asm("edx");

	switch(call_number){
		case 1:
		case 2:
		case 3:
			read(arg1, (void*)arg2, arg3);				/*LOOK AT THIS NOTE: We need to catch the return values and place them in EAX*/
			break;
		case 4:
			write(arg1, (void*)arg2, arg3);
			break;
		case 5:
			open((uint8_t*)arg1);
			break;
		case 6:
			close(arg1);
			break;
		case 7:
		case 8:
		case 9:
		case 10:
		default:
			printf("Invaild call number");
			break;
	};       

}

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
	if(nbytes < 0 || buf == NULL) return -1;	
	
	switch(fd){
		case STDIN:
			return keyboard_read(buf, nbytes);
			break;

		case STDOUT:
			return terminal_read(buf, nbytes);
			break;
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
	if(nbytes < 0 || buf == NULL) return -1;	

	switch(fd){
		case STDIN:
			return keyboard_write(buf, nbytes);
			break;

		case STDOUT:
			return terminal_write(buf, nbytes);
			break;

	};

	return -1;
	/*int i;
	
	if(fd == STDIN){ //change fd plus they wil call this
		int idx[2];
		get_keyboard_idx(idx);
		if(idx[0] != idx[1]){						//this
			for(i = idx[0]; i < idx[1]; i++){
				char data = ((char *)buf)[i];
				putc(data);
			}
			return(idx[1] - idx[0]);
		}
	} else if(fd == STDOUT){
		if(strlen(buf) < nbytes){
			for(i = 0; i < strlen(buf); i++) {
				char data = ((char *)buf)[i];
				putc(data);
			}	
			return strlen(buf);
		}
		for(i = 0; i < nbytes; i++){
			char data = ((char *)buf)[i];
			putc(data);
		}
		return nbytes;
	}

	return -1;*/
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
	return 0;
}

 int32_t execute (const uint8_t* command) {
 	cli();
 	//need to remember parent's PID, incrementing on execute & decrementing on halt
 	uint32_t process_num = 0; //change for next checkpoint. Maybe change for shell
 	uint8_t buf[1024];
 	uint8_t com[1024];
 	uint8_t buffer[4];
 	char* args[1024];
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
 		sti();
 		return -1;
 	}
 	//check for valid exe
 	read_data(valid_file_check.inode, 0, buffer, 4);
 	if (buffer[0] != 0x7F || buffer[1] != 0x45 || buffer[2] != 0x4C || buffer[3] != 0x46){
 		sti();
 		return -1;
 	}
 	// get start address
 	read_data(valid_file_check.inode, 24, buffer, 4);
 	file_start = *((uint32_t*)buffer);


 	// map new page
 	map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * process_num);

 	// write file into memory
 	read_data(valid_file_check.inode, 0, (uint8_t*)VIRTUAL_FILE_START, 100000);

 	tss.ss0 = KERNEL_DS;
 	tss.esp0 = PHYS_FILE_START - EIGHT_KB * (process_num) - 4;
 	jump_user_space(file_start);
 	// sti();
 	 asm volatile(
      			// "cli;"
       			 "mov $0x2B, %%ax;" // 0x2B 0x23
                 "mov %%ax, %%ds;"
                 "mov %%ax, %%es;"
                 "mov %%ax, %%fs;"
                 "mov %%ax, %%gs;"
                 "movl  $0x83FFFFC, %%eax;" //final memory address of file's pageu
                 "pushl $0x2B;" //0x2B 0x23
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;" //enable interrupts and push flags
                 "pushl %%edx;"
                 "pushl $0x2B;" //0x23 0x1B
                 "pushl %0;" 
                 "iret;"
                 "HALTED:;"
                 "leave;"
                 "ret;"
                 :	/* no outputs */
                 :"r"(file_start)	/* input */
                 :"%edx","%eax"	/* clobbered register */
                 );
    asm volatile(
      			// "cli;"
       			 "mov $0x23, %%ax;" // 0x2B 0x23
                 "mov %%ax, %%ds;"
                 "movl $0x83FFFFC, %%eax;"
                 "pushl $0x23;" //0x2B 0x23
                 "pushl %%eax;"
                 "pushfl;"
                 "popl %%edx;"
                 "orl $0x200, %%edx;" //enable interrupts and push flags
                 "pushl %%edx;"
                 "pushl $0x1B;" //0x23 0x1B
                 "pushl %0;" 
                 "iret;"
                 "HALTED1:;"
                 "leave;"
                 "ret;"
                 :	/* no outputs */
                 :"r"(file_start)	/* input */
                 :"%edx","%eax"	/* clobbered register */
                 );

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


