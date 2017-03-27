#include "syscall.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/*
void system_call_handler()
  Input: none
  Return Value: none 
  Function: Calls the apropriate system call based in the argumet registers. The call number is found in EAX
*/
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
			read(arg1, (void*)arg2, arg3);
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
	if(fd == STDIN){
		while(enter == 0){				//wait until enter is pressed
			//wait
		};

		int idx[2];
		int i;
		int j = 0;						//holds the index of the first char to copy to the buffer
		int count = 0;					//hold the number of new lines seen so far
		get_keyboard_idx(idx);			//get the idecies of the keyboard

		for(i = (idx[1]-1); i >= 0; i--){	//start at the right end of the buffer and go until 0
			if(keyboard_buf[i] == '\n'){
				count++;
				if(count == 2){			//if the new line count is 2 then youre done
					j = i+1;
					break;
				}
			}
		}
		for(i = j; keyboard_buf[i] != '\0'; i++){		//copys the keyboard buffer to the given buffer
			if((i-j) == nbytes){
				break;
			}
			((char *)buf)[i] = keyboard_buf[i];
		}
		enter = 0;					//set the volatile enter to zero
		return (i-j);				//the number of bytes read
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
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	if(nbytes < 0) return -1;			//check for vaild args
	if(buf == NULL) return -1;

	int i;
	
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

	return -1;
}

/*
int32_t open(const uint8_t* filename)
  Input: filename - the name of the file to open
  Return Value: returns 0 on sucsess, or -1 on failure
  Function: opens a file
*/
int32_t open(const uint8_t* filename){
	return 0;
}

/*
int32_t close(const uint8_t* filename)
  Input: filename - the name of the file to close
  Return Value: returns 0 on sucsess, or -1 on failure
  Function: closes a file
*/
int32_t close(int32_t fd){
	if(fd == STDIN){
		disable_irq(KEYBOARD_IRQ);
	}else if(fd == STDOUT){
		return 0;
	}
	return -1;
}
