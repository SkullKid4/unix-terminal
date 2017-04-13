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
#define NUM_PROCESSES 6
#define MAX_FILE_CHAR 32


#define NOT_SET 0
#define STDINFLAG 1
#define STDOUTFLAG 2
#define RTCFLAG 3
#define FILEFLAG 4
#define DIRFLAG 5
#define FILE_START 0


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

uint32_t = curr_process;

typedef struct fops {
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
} fops_t;


typedef struct fds{
	uint32_t jump_table_pointer;
    uint32_t inode;
	uint32_t file_position;
	uint32_t flags;
} fds_t;

fds_t my_fds[8];

typedef struct pcb{
  uint32_t PID;
  fds_t FDs_array[8]; //<----------- array of fds structs?
  uint8_t file_name[MAX_FILE][MAX_FILE_CHAR]
  uint32_t ESP0;
  uint32_t PPID;
  uint32_t ARGS;
} pcb_t;



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

int32_t getprocess();
void invalid_function();
void end_process(int32_t proc_num);
void do_nothing();



#endif
