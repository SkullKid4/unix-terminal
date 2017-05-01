#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"
#include "files.h"
#include "rtc.h"
#include "keyboard.h"
#include "types.h"
#include "paging.h"
#include "terminal.h"
#include "scheduler.h"

#define VIDEO 0xB8000
#define ATTRIB 0x7
int32_t curr_terminal_number = 0;
// int32_t current_executing_terminal = 0;
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
			if (get_curr_exec_term() == get_cur_term())
          		putc(data);
        	else
          		putc_nodisplay(data);
		}	
		//tlock = 0;
		sti();
		return strlen(buf);
	}
	for(i = 0; i < nbytes; i++){
		char data = ((char *)buf)[i];
		if (get_curr_exec_term() == get_cur_term())
          putc(data);
        else
          putc_nodisplay(data);
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
		terminals[i].term_enter = 0;
		terminals[i].keyboard_last_index = 0;
		terminals[i].keyboard_index = 0;
		memset(terminals[i].input_buf, ' ', MAX_BUF_SIZE);
		
		for(j=0; j<NUM_ROWS*NUM_COLS; j++) {
        	*(uint8_t *)(terminals[i].screen + (i << 1)) = '\0';
       	 	*(uint8_t *)(terminals[i].screen + (i << 1) + 1) = ATTRIB;
    	}
	
	}
}

void switch_terminal(int32_t newt)  {
	if(newt == curr_terminal_number){
		return;
	}
	if(newt > 2){
		return;
	}
	if(terminals[newt].active == 0){
		save_terminal_state();
		pcb_t* old_pcb = get_pcb_pointer(terminals[curr_terminal_number].current_process);
		//old_pcb->FDs_array[2].flags = RTCFLAG;
		curr_terminal_number = newt;
		    /* Save the ebp/esp of the process we are switching away from. */
	    asm volatile("			\n\
	                 movl %%ebp, %%eax 	\n\
	                 movl %%esp, %%ebx 	\n\
	                 "
	                 :"=a"(old_pcb->EBP_SWITCH), "=b"(old_pcb->ESP_SWITCH)
		);
		//sti();
		set_curr_exec_term(newt);
		clear();
		execute((uint8_t*)("shell\0"));
		return;
	}

	save_terminal_state();
	restore_terminal_state(newt);
	curr_terminal_number = newt;
	uint8_t * screen_start;
    vidmap(&screen_start);
	if(get_curr_exec_term() != curr_terminal_number){
		map_video_w_pt((uint32_t)screen_start, (uint32_t)terminals[curr_terminal_number].screen);
	}
	return;


	// pcb_t* old_pcb = get_pcb_pointer(terminals[curr_terminal_number].current_process);
	// //old_pcb->FDs_array[2].flags = RTCFLAG;
	// curr_terminal_number = newt;

	// 	    /* Save the ebp/esp of the process we are switching away from. */

 //    pcb_t* new_pcb = get_pcb_pointer(terminals[newt].current_process);
 //    current_pcb = new_pcb;
 //    set_process_sys(terminals[newt].current_process);
	// map(VIRTUAL_FILE_PAGE, PHYS_FILE_START+PHYS_FILE_OFFSET*terminals[newt].current_process);
 // 	tss.ss0 = KERNEL_DS;
 // 	tss.esp0 = PHYS_FILE_START - (EIGHT_KB * terminals[newt].current_process) - 4;


 //    asm (
 // 	"movl	%%cr3,%%eax ;"
	// "movl	%%eax,%%cr3 "
	// : : :"eax", "cc");
 //        asm volatile("			\n\
 //                 movl %%ebp, %%eax 	\n\
 //                 movl %%esp, %%ebx 	\n\
 //                 "
 //                 :"=a"(old_pcb->EBP_SWITCH), "=b"(old_pcb->ESP_SWITCH)
	// );
 //        asm volatile(
	// 			 ""
 //                 "mov %0, %%esp;"
 //                 "mov %1, %%ebp;"
 //                 //"jmp HALTED;"
 //                 :                       /*no outputs */
 //                 :"r"(new_pcb->ESP_SWITCH), "r"(new_pcb->EBP_SWITCH)    /*inputs */
 //                 :"%eax"                /*  clobbered registers */
 //                 );
	//	return;
	
	//clear();
	//print screen text

}


void save_terminal_state(){
	terminals[curr_terminal_number].x = screen_x;
	terminals[curr_terminal_number].y = screen_y;
	memcpy(terminals[curr_terminal_number].screen, video_mem, 2 *NUM_ROWS * NUM_COLS);
	memcpy(terminals[curr_terminal_number].input_buf, keyboard_buf, MAX_BUF_SIZE);
	terminals[curr_terminal_number].keyboard_last_index = get_keyboard_last_index();
	terminals[curr_terminal_number].keyboard_index = get_keyboard_index();

	/*int i;
	for(i = 0; i < MAX_BUF_SIZE+1; i++) {
		terminals[curr_terminal_number].input_buf[i] = keyboard_buf[i];
	}*/
}

void restore_terminal_state(int newt){
	memcpy(video_mem, terminals[newt].screen, 2 * NUM_ROWS * NUM_COLS);
	screen_x = terminals[newt].x;
	screen_y = terminals[newt].y;
	update_cursor(screen_y, screen_x);
	memcpy(keyboard_buf, terminals[newt].input_buf, MAX_BUF_SIZE);
	set_keyboard_last_index(terminals[newt].keyboard_last_index);
	set_keyboard_index(terminals[newt].keyboard_index);
	/*int i;
	for(i = 0; i < MAX_BUF_SIZE+1; i++) {
		keyboard_buf[i] = terminals[newt].input_buf[i];
	}*/
}
void set_curr_process(int process_number){
	terminals[get_curr_exec_term()].current_process =  process_number;
	// terminals[curr_terminal_number].current_process =  process_number;
}

int get_cur_term(){
	return curr_terminal_number;
}
