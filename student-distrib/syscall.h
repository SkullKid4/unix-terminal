#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define STDIN	0
#define STDOUT	1

/*dispacture for the system calls*/
extern void system_call_handler();

/*function for reading data to buffers*/
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
/*function to writing to buffers*/
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);
/*opens files*/
extern int32_t open(const uint8_t* filename);
/*closes files*/
extern int32_t close(int32_t fd);


#endif
