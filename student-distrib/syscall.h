#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define VIDEO 0xB8000

extern void system_call_handler();
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);

#endif
