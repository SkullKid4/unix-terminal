#include "syscall.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

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

int32_t read(int32_t fd, void* buf, int32_t nbytes){
	if(fd == STDIN){
		while(enter == 0){
			//wait
		};

		int idx[2];
		int i;
		int j = 0;
		int count = 0;
		get_keyboard_idx(idx);

		for(i = (idx[1]-1); i >= 0; i--){
			if(keyboard_buf[i] == '\n'){
				count++;
				if(count == 2){
					j = i+1;
					break;
				}
			}
		}
		for(i = j; keyboard_buf[i] != '\0'; i++){
			if((i-j) == nbytes){
				break;
			}
			((char *)buf)[i] = keyboard_buf[i];
		}
		enter = 0;
		return (i-j);
	}
	return -1;
}

/*
int32_t write
  INPUT: fd - file descriptor, indicates where this is being called from
  		buf - the buffer to write
  		nbytes - number of bytes to write
  Return Value: returns number of bytes written, or -1 on failure
  Function: Writes a number of bytes from a buffer, according to the file descriptor
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	if(nbytes < 0) return -1;
	if(buf == NULL) return -1;
	if(fd == STDOUT){ //change fd plus they wil call this
		int i;
		int idx[2];
		get_keyboard_idx(idx);
		if(idx[0] != idx[1]){
			for(i = idx[0]; i < idx[1]; i++){
				char data = ((char *)buf)[i];
				putc(data);
			}
			return(idx[1] - idx[0]);
			
		} else{
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
	}
	return -1;
}


int32_t open(const uint8_t* filename){
	return 0;
}


int32_t close(int32_t fd){
	if(fd == STDIN){
		disable_irq(KEYBOARD_IRQ);
	}else if(fd == STDOUT){
		return 0;
	}
	return -1;
}
