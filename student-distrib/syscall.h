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
#define MAX_FILE 8


#define NOT_SET 0
#define STDINFLAG 1
#define STDOUTFLAG 2
#define RTCFLAG 3
#define FILEFLAG 4
#define DIRFLAG 5
#define FILE_START 0


volatile uint32_t curr_process;

typedef struct fops {
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
} fops_t;


typedef struct fds{
	uint32_t* jump_table_pointer;
  uint32_t inode;
	uint32_t file_position;
	uint32_t flags;
} fds_t;

fds_t my_fds[8];
uint32_t process_array[6];

typedef struct pcb{
  uint32_t PID;
  fds_t FDs_array[8]; //<----------- array of fds structs?
  int8_t file_name[MAX_FILE][MAX_FILE_CHAR];
  uint32_t ESP0;
  uint32_t PPID;
  uint32_t ARGS;
  uint32_t EBP0;
} pcb_t;



/*dispacture for the system calls*/

/*function for reading data to buffers*/
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);
/*function to writing to buffers*/
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
/*opens files*/
extern int32_t open(const uint8_t* filename);
/*closes files*/
extern int32_t close(int32_t fd);


extern int32_t halt(uint8_t status);
extern int32_t halt_from_exc();
extern int32_t execute(const uint8_t* command);
extern int32_t getargs(uint8_t* buf, int32_t nbytes);
extern int32_t vidmap(uint8_t** sreen_start);
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);

int32_t get_process();
int32_t invalid_function();
void end_process(int32_t proc_num);
int32_t do_nothing();
void clear_process();


#endif
