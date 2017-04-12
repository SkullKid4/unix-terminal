#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define VIDEO 0xB8000
#define NUM_COLS 80
#define NUM_ROWS 25

#define KEYBOARD_IRQ			1		//this is the irq number on the master PIC
#define KEYBOARD_STATUS_PORT	0x64		//port for reading keyboard stsatus
#define KEYBOARD_DATA_PORT		0x60		//port for reading keybored data
#define KEYBOARD_IDT_IDX		0x21		//the keyboreds index into the idt
#define MAX_PRESS_CODE			0x7F
#define SHIFT_DOWN_L			0x2A
#define SHIFT_UP_L				0xAA
#define SHIFT_UP_R				0xB6
#define SHIFT_DOWN_R			0x36
#define CAPS_DOWN				0x3A
#define CTRL_DOWN				0x1D
#define CTRL_UP					0x9D
#define BACKSPACE				0x0E

#define MAX_BUF_SIZE			128

volatile unsigned enter;

char keyboard_buf[MAX_BUF_SIZE+1];   //128 + 1 for end of string
char one_line_buf[NUM_COLS+1]; 

void get_keyboard_idx(int* data);

extern int32_t keyboard_read(void* buf, int32_t nbytes);
extern int32_t keyboard_write(void* buf, int32_t nbytes);
extern int32_t keyboard_close();
/* function that is called from the idt table when interrupts occor*/
void keyboard_handler();

/*sets the idt entry with the handdlers adrs*/
void keyboard_init();

void handle_backspace();
//jump table holder for the function
void none();


#endif
