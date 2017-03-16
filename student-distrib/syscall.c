#include "syscall.h"
#include "keyboard.h"
#include "lib.h"

void system_call_handler()
{
	//register int ecx asm("ecx")

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
			break
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
	if(fd == VIDEO){
		int idx[2];
		int i;
		int j = 0;
		get_keyboard_idx(idx);

		for(i = (idx[1] - 2); i >= 0; i--){
			if(keyboard_buf[i] == '\n'){
				j = i+1;
				break;
			}
		}
		for(i = j; keyboard_buf[i] != '\0'; i++){
			if((i-j) == nbytes){
				break;
			}
			((char *)buf)[i] = keyboard_buf[i];
		}
		return (i-j);
	}
return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	if(fd == VIDEO){
		int i;
		int idx[2];
		get_keyboard_idx(idx);
		for(i = idx[0]; i < idx[1]; i++){
			char data = ((char *)buf)[i];
			putc(data);
		}
	}
	//gerneral case for terminal output
	return 0;
}

/*
terminal:
	return 0

keyborad:
	enable irq
*/
int32_t open(const uint8_t* filename){
	if()
}
/*
terminal:
	return -1

keyborad:
	diasble irq
*/
int32_t close(int32_t fd){
	if(fd == VIDEO){

	}
}
