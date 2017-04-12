#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define STDIN	0
#define STDOUT	1

#define IN_USE 1
#define NOT_IN_USE 0
#define STDIN 0
#define STDOUT  1
#define PHYS_FILE_START 0x800000
#define VIRTUAL_FILE_PAGE 0x8000000
#define PHYS_FILE_OFFSET 0x400000
#define VIRTUAL_FILE_START 0x8048000
#define EIGHT_KB 8192

#define DO_CALL(name, number, arg1, arg2, arg3)       \
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

typedef struct fds{
	uint32_t jump_table_pointer;
    uint32_t inode;
	uint32_t file_position;
	uint32_t flags;
} fds_t;

fds_t my_fds[8];


/*dispacture for the system calls*/
extern void system_call_handler();

/*function for reading data to buffers*/
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
/*function to writing to buffers*/
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
/*opens files*/
extern int32_t open(const uint8_t* filename);
/*closes files*/
extern int32_t close(int32_t fd);


extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t* command);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** sreen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);


#endif
