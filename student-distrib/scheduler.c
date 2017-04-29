#include "scheduler.h"
#include "paging.h"
#include "syscall.h"
#include "x86_desc.h"
#include "i8259.h"
#include "pit.h"
#include "terminal.h"

volatile uint8_t curr_task= 0;
volatile uint8_t curr_term = 0;
volatile uint8_t last_term = 0;
volatile uint8_t last_task = 0;

void switch_task(){
   /* copy the saved registers into the current_proc structure */
    //uint8_t count =0;

      last_task = curr_task;
      last_term = curr_term;
      curr_term++;	   
      send_eoi(PIT_IRQ);
      while(terminals[curr_term].active == 0){
		   curr_term++;
         //count++;
         if(curr_term > 2){
            curr_term = 0;
         }
      }
      while(terminals[curr_term].ESP == 0){}
	   curr_task = terminals[curr_term].current_process;	  

      tss.esp0 = PHYS_FILE_START - EIGHT_KB * (curr_task) - 4;
      map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * curr_task);
      if(curr_term == get_cur_term()) {
         map_w_pt(USER_VID_MEM, VIDEO);
      }else{
         map_w_pt(USER_VID_MEM, VIDEO + (VIDEO_OFFSET * (curr_task + 1)));
      }
      // register int temp asm("esp");
      // task_array[prev_idx].ESP0 = temp;
      //save then restore
      asm volatile("       \n\
            movl %%ebp, %%eax    \n\
            movl %%esp, %%ebx    \n\
      "
      :"=a"(terminals[last_term].EBP), "=b"(terminals[last_term].ESP));

      asm volatile(""
           "mov %0, %%esp;"
           "mov %1, %%ebp;"
           //"jmp HALTED;"
           :                      /* no outputs */
           :"r"(terminals[curr_term].ESP), "r"(terminals[curr_term].EBP)   /* inputs */ 
      );
}

/*void scheduler_init(){
   int i;
   for(i = 0; i < 6; i++){
      if(process_array[i]==1){
		  curr_task=i;
		  break;
	  }
   }
}*/
/*
void add_task(int esp0, int pid){
   int i;
   int temp_idx;
   if(task_lock==0){
	task_lock=1;
   cli();
   for(i = 0; i < 6; i++){
      if(task_array[i].active == 0){
         temp_idx = i;
         break;
      }
   }
   task_array[temp_idx].active = 1;
   task_array[temp_idx].ESP0 = esp0;
   task_array[temp_idx].PID = pid;
	sti();
	task_lock=0;
   }
}
void remove_task(int pid){
   int i;
   if(task_lock==0){
   task_lock=1;
   cli();
   for(i = 0; i < 6; i++){
      if(task_array[i].PID == pid){
         task_array[i].active = 0;
         break;
      }
   }
   sti();
   task_lock=0;
   }
   
}
*/
