#ifndef SYSCALL_LINK_H
#define SYSCALL_LINK_H
#include "keyboard.h"
#include "pit.h"

/*
int syscall_link(int cmd, int arg1, int arg2, int arg3)
  Input: 
  	cmd: indicates which system call to call
  	int arg1, int arg2, int arg3: vary depending on the system call
  Return Value: -1 on failure or returns the value passed from the return from the system call
  Function: linkage from the system call itself to the actual implimentation
*/
int syscall_link(int cmd, int arg1, int arg2, int arg3);

/*
void jump_user_space(int entry_point)
  Input: 
  	int entry_point: the start of the user program
  Return Value: none
  Function: arranges arguments on the stack for iret
*/
void jump_user_space(int entry_point);
/*
void rtc_linkage
  Input: None
  Return Value: none
  Function: save all registers and pop all registers before calling rtc interrupt handlers
*/
void rtc_linkage();
/*
void keyboard_handler
  Input: None
  Return Value: none
  Function: save all registers and pop all registers before calling keyboard interrupt handlers
*/
void keyboard_linkage();

void pit_linkage();

#endif
