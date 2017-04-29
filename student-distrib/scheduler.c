#include "scheduler.h"
#include "paging.h"
#include "syscall.h"
#include "x86_desc.h"
#include "i8259.h"
#include "pit.h"

volatile uint8_t task_idx = 0;
volatile uint8_t task_lock = 0;

void switch_task(){
   /* copy the saved registers into the current_proc structure */
   if(task_lock == 0){
      task_lock = 1;
      cli();
      uint8_t temp_idx = task_idx;
      uint8_t count =0;
      
      /* now go onto the next task - if there isn't one, go back to the start
         of the queue. */
      while(task_array[task_idx].active == 0){
         task_idx++;
         count++;
         if(task_idx >= 6){
            task_idx = 0;
         }
        if(count>6){
            sti();
            task_lock = 0;
            send_eoi(PIT_IRQ);
            return;
        }
      }
      task_idx++;
      
      tss.esp0 = PHYS_FILE_START - EIGHT_KB * (task_array[task_idx].PID) - 4;
      map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * task_array[task_idx].PID);
      register int temp asm("esp");
      task_array[temp_idx].ESP0 = temp;
      sti();
      task_lock = 0;
      send_eoi(PIT_IRQ);
   }
}

void scheduler_init(){
   int i;
   for(i = 0; i < 6; i++){
      task_array[i].active = 0;
   }
}

void add_task(int esp0, int pid){
   int i;
   int temp_idx;
   for(i = 0; i < 6; i++){
      if(task_array[i].active == 0){
         temp_idx = i;
         break;
      }
   }
   task_array[temp_idx].active = 1;
   task_array[temp_idx].ESP0 = esp0;
   task_array[temp_idx].PID = pid;
}

void remove_task(int pid){
   int i;
   for(i = 0; i < 6; i++){
      if(task_array[i].PID == pid){
         task_array[i].active = 0;
         break;
      }
   }
}
