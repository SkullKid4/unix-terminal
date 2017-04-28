#include "scheduler.h"

void switch_task(registers_t *regs){
   /* copy the saved registers into the current_proc structure */
   memcpy(&current_proc->regs, regs, sizeof(registers_t));
 
   /* now go onto the next task - if there isn't one, go back to the start
      of the queue. */
   if (current_proc->next != NULL){
   		current_proc = current_proc->next;
   }else{
		current_proc = pqueue;
   }
   /* now hack the registers! */
   memcpy(regs, &current_proc->regs, sizeof(registers_t));
   //map?				switch_page_directory(current_proc->cr3);
}