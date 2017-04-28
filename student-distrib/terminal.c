#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"
#include "rtc.h"
#include "keyboard.h"
#include "types.h"
#include "terminal.h"

#define VIDEO 0xB8000
#define ATTRIB 0x7
int32_t curr_terminal_number = 0;
int32_t current_executing_terminal = 0;
static char* video_mem = (char *)VIDEO;

/*
int32_t terminal_read
  Input: buf - buffer to read to
  		nbytes - number of bytes to read
  Return Value: -1
  Function: does nothing (shouldn't be called)
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	return -1;
}

/*
int32_t terminal_write
  Input: buf - buffer to write from
  		nbytes - number of bytes to write
  Return Value: number of bytes read
  				-1 on failure
  Function: writes the contents of the buffer to the terminal
*/
//volatile unsigned tlock = 0;

int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes) {
	//while(tlock == 1){}
	//tlock = 1;
	if(buf == NULL || nbytes < 0) return -1;
	cli();
	int i;
	if(strlen(buf) < nbytes){
		for(i = 0; i < strlen(buf); i++) {
			char data = ((char *)buf)[i];
			putc(data);
		}	
		//tlock = 0;
		sti();
		return strlen(buf);
	}
	for(i = 0; i < nbytes; i++){
		char data = ((char *)buf)[i];
		putc(data);
	}
	//tlock = 0;
	sti();
	return nbytes;
}
/*
int32_t terminal_open
  Input: filename-be consistent with open syscalls prototype
  Return Value: 0
  Function: does nothing (shouldn't be called)
*/

int32_t terminal_open(const uint8_t* filename) {
	return 0;
}
/*
int32_t terminal_close
  Input: none
  Return Value: 0
  Function: does nothing (shouldn't be called)
*/
int32_t terminal_close() {
	return 0;
}

void terminal_init() {
	int i, j;
	curr_terminal_number = 0;
	for (i = 0; i < 3; i++) {
		terminals[i].current_process = -1;
		terminals[i].active = 0;
		terminals[i].x = 0;
		terminals[i].y = 0;
		
		for(j=0; j<NUM_ROWS*NUM_COLS; j++) {
        	*(uint8_t *)(terminals[i].screen + (i << 1)) = '\0';
       	 	*(uint8_t *)(terminals[i].screen + (i << 1) + 1) = ATTRIB;
    	}
	
	}
}

void switch_terminal(int32_t newt)  {
	cli();
	if(newt == curr_terminal_number){
		sti();
		return;
	}
	if(newt > 2){
		sti();
		return;
	}
	if(terminals[newt].active == 0){
		save_terminal_state();
		clear();
		//terminals[newt].active = 1;
		pcb_t* old_pcb = get_pcb_pointer(terminals[curr_terminal_number].current_process);
		curr_terminal_number = newt;

		    /* Save the ebp/esp of the process we are switching away from. */
    asm volatile("			\n\
                 movl %%ebp, %%eax 	\n\
                 movl %%esp, %%ebx 	\n\
                 "
                 :"=a"(old_pcb->EBP0), "=b"(old_pcb->ESP0)
	);
	sti();
	//execute((uint8_t*)"shell");
	return;
	}

	save_terminal_state();
	restore_terminal_state(newt);
	update_cursor(screen_x, screen_y);
	//key_buffer = terms[term_id].key_buffer;
	//current_term_id = term_id;
        /* Remap video memory to 136 MB */
        //uint8_t * screen_start;
       // vidmap(&screen_start);
        //if (terms[current_term_executing].id != current_term_id) {
         //   remapVideoWithPageTable((uint32_t)screen_start, (uint32_t)terms[current_term_executing].video_mem);
        //}
	pcb_t* old_pcb = get_pcb_pointer(terminals[curr_terminal_number].current_process);
	curr_terminal_number = newt;

		    /* Save the ebp/esp of the process we are switching away from. */
    asm volatile("			\n\
                 movl %%ebp, %%eax 	\n\
                 movl %%esp, %%ebx 	\n\
                 "
                 :"=a"(old_pcb->EBP0), "=b"(old_pcb->ESP0)
	);
    pcb_t* new_pcb = get_pcb_pointer(terminals[newt].current_process);
        asm volatile(
				 ""
                 "mov %0, %%esp;"
                 "mov %1, %%ebp;"
                 "leave;"
                 "ret;"
                 //"jmp HALTED;"
                 :                      /* no outputs */
                 :"r"(new_pcb->ESP0), "r"(new_pcb->EBP0)   /* inputs */ 
                 :"%eax"                 /* clobbered registers */
                 );


		return;
	
	//clear();
	//print screen text

}


void set_terminal_x(int x_location) {
	terminals[curr_terminal_number].x = x_location;
}


void set_terminal_y(int y_location) {
	terminals[curr_terminal_number].y = y_location;
}

int get_terminal_x() {
	return terminals[curr_terminal_number].x;
}

int get_terminal_y() {
	return terminals[curr_terminal_number].y;
}

void save_terminal_state(){
	terminals[curr_terminal_number].x = screen_x;
	terminals[curr_terminal_number].y = screen_y;
	memcpy(terminals[curr_terminal_number].screen, video_mem, 2 *NUM_ROWS * NUM_COLS);
}

void restore_terminal_state(int newt){
	memcpy(video_mem, terminals[newt].screen, 2 * NUM_ROWS * NUM_COLS);
	screen_x = terminals[newt].x;
	screen_y = terminals[newt].y;
}
void set_curr_process(int process_number){
	terminals[curr_terminal_number].current_process =  process_number;
}

int get_cur_term(){
	return curr_terminal_number;
}
