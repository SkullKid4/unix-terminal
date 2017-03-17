#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define STDIN	0
#define STDOUT	1

extern void system_call_handler();
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t open(const uint8_t* filename);
extern int32_t close(int32_t fd);


#endif
