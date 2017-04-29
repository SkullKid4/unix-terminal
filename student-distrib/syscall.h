#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib.h"

#define IN_USE 1
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
#define USER_VID_MEM 0x9000000
#define VIDEO 0xB8000
#define RTCIDX 2


#define NOT_SET 0
#define STDINFLAG 1
#define STDOUTFLAG 2
#define RTCFLAG 3
#define FILEFLAG 4
#define DIRFLAG 5
#define FILE_START 0
#define BUF_SIZE 128
#define SMALL_BUF 4
#define READ_WHOLE_FILE 100000


volatile uint32_t curr_process;
/*file struct pointers for read and write*/
typedef struct fops {
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, void* buf, int32_t nbytes);
} fops_t;

fops_t fops_table[6]; //NOT_SET, STDIN, STDOUT, RTC, File, Directory

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
  fds_t FDs_array[8]; 
  int8_t args[BUF_SIZE];
  int8_t file_name[MAX_FILE][MAX_FILE_CHAR];
  uint32_t ESP0;
  uint32_t ESP_SWITCH;
  uint32_t EBP_SWITCH;
  uint32_t PPID;
  uint32_t ARGS;
  uint32_t EBP0;
} pcb_t;

volatile pcb_t* current_pcb;


extern void init_fops_table();
/*function for reading data to buffers*/
extern int32_t read(int32_t fd, void* buf, int32_t nbytes);

int32_t file_read_setup(int32_t fd, void* buf, int32_t nbytes);

/*function to writing to buffers*/
extern int32_t write(int32_t fd, void* buf, int32_t nbytes);
/*opens files*/
extern int32_t open(const uint8_t* filename);
/*closes files*/
extern int32_t close(int32_t fd);

/*CLOSES A PROGRAM BY RESTORING PARENT DATA*/
extern int32_t halt(uint8_t status);

/*CLOSES A PROGRAM BY RESTORING PARENT DATA when a execption is given*/
extern int32_t halt_from_exc();

/*loads a program into memory and gives control to that program*/
extern int32_t execute(const uint8_t* command);

/*get the arguments of the user program*/
extern int32_t getargs(uint8_t* buf, int32_t nbytes);

/*loads video memory into user space*/
extern int32_t vidmap(uint8_t** screen_start);

/*sets a signal handler to a user-specified funciton*/
extern int32_t set_handler(int32_t signum, void* handler_address);

/*copies hardware context from user-level stack to processor*/
extern int32_t sigreturn(void);

/*used to parse running processes and find a space for the process to run, asserts a maximum number of processes*/
int32_t get_process();

/*not implimented*/
int32_t invalid_function(int32_t fd, void* buf, int32_t nbytes);

/*sets the entry in process_array for the process to 0, indicating that another process may be run in its place*/
void end_process(int32_t proc_num);

/*not i plimented*/
int32_t do_nothing();

/*sets all entries in process_array to 0, used before executing shell in kernel.c*/
void clear_process();

uint32_t get_current_process();

pcb_t* get_pcb_pointer(int process);

void set_process_sys(int process);

#endif
