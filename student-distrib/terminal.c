#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"
#include "rtc.h"
#include "keyboard.h"
#include "types.h"

int32_t terminal_read(void* buf, int32_t nbytes) {
	return 0;
}

int32_t terminal_write(void* buf, int32_t nbytes) {
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

int32_t terminal_open() {
	return 0;
}

int32_t terminal_close() {
	return 0;
}

