#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"
#include "syscall.h"
#include "keyboard.h"

typedef struct terminal {
	uint8_t active;
	int current_process;
	int x;
	int y;
	pcb_t* pcb;
	char input_buf[MAX_BUF_SIZE+1];
	char screen[NUM_ROWS * NUM_COLS];
} terminal_t;

terminal_t terminals[3];

/*does nothing (shouldn't be called)*/
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

/*writes the contents of the buffer to the terminal*/
extern int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes);

extern int32_t terminal_open();
extern int32_t terminal_close();
extern void switch_terminal(int32_t newt);
void save_terminal_state();

void restore_terminal_state(int newt);

int get_cur_term();

void set_curr_process(int process_number);


#endif
