#include "scheduler.h"
#include "paging.h"
#include "syscall.h"
#include "x86_desc.h"
#include "i8259.h"
#include "pit.h"
#include "terminal.h"

volatile uint8_t curr_task= 0;
volatile uint8_t task_lock = 0;
volatile uint8_t curr_term = 0;
volatile uint8_t last_task;
void switch_task(){
   /* copy the saved registers into the current_proc structure */
    //uint8_t count =0;

   if(task_lock == 0){
      task_lock = 1;
      cli();
	  send_eoi(PIT_IRQ);
	   
	  
      //uint8_t temp_idx = task_idx;
      /* now go onto the next task - if there isn't one, go back to the start
         of the queue. */

      while(terminals[curr_term].active == 0){
		 curr_term++;
         //count++;
         if(curr_term >= 2){
            curr_term = 0;
         }
         /*if(count>=3){
			sti();
			task_lock=0;
			return;
        }*/
      }
	  curr_task=terminals[curr_term].current_process;
	  if(last_task==curr_task){
		  sti();
		  curr_term++;
		  task_lock=0;
		  return;
	  }		  
	  //printf("after loop active? %u, %u",curr_task, process_array[curr_task]);
      //pcb_t* curr_pcb=get_pcb_pointer(curr_task);
	  //tss.esp0 = PHYS_FILE_START - EIGHT_KB * (curr_task) - 4;
      tss.esp0=terminals[curr_term].pcb->ESP0;
	  //if(curr_term==get_cur_term())
		  
	  map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * (terminals[curr_term].pcb->PID));
      //register int temp asm("esp");
      //task_array[temp_idx].ESP0 = temp;
	  last_task=curr_task;
	  curr_term++;
	  sti();
      task_lock = 0;
   }
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
