#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define STDIN	0
#define STDOUT	1

#define DO_CALL(name,number, arg1, arg2, arg3)       \
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
")
  //LEAVE						;\
  //RET                        \
//  CMP $0xFFFFC000,%EAX  ;\
//  JBE 1                 ;\
//  MOVL  $-1,%EAX    ;\

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
