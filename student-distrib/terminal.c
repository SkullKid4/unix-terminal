#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"
#include "rtc.h"
#include "keyboard.h"
#include "types.h"


/*
int32_t terminal_read
  Input: buf - buffer to read to
  		nbytes - number of bytes to read
  Return Value: 0
  Function: does nothing (shouldn't be called)
*/
int32_t terminal_read(void* buf, int32_t nbytes) {
	return 0;
}

/*
int32_t terminal_write
  Input: buf - buffer to write from
  		nbytes - number of bytes to write
  Return Value: number of bytes read
  				-1 on failure
  Function: 
*/
int32_t terminal_write(void* buf, int32_t nbytes) {
	if(buf == NULL || nbytes < 0) return -1;
	int i;
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


/*int32_t terminal_open() {
	return 0;
}

int32_t terminal_close() {
	return 0;
}*/

