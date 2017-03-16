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
		case 6:
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
		//int idx = get_keyboard_idx();
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
return 0;
}
