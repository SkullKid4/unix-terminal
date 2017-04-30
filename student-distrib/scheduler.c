#include "scheduler.h"
#include "paging.h"
#include "syscall.h"
#include "x86_desc.h"
#include "i8259.h"
#include "pit.h"
#include "terminal.h"
#include "lib.h"

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
      //while(terminals[curr_term].ESP == 0){}
      terminals[last_term].ESP0 = tss.esp0;
      curr_task = terminals[curr_term].current_process;    
      pcb_t* old_pcb = get_pcb_pointer(terminals[last_term].current_process);
      pcb_t* new_pcb = get_pcb_pointer(terminals[curr_term].current_process);

      tss.esp0 = terminals[curr_term].ESP0;
      map(VIRTUAL_FILE_PAGE, PHYS_FILE_START + PHYS_FILE_OFFSET * curr_task);

     // &screen_x = &terminals[last_term].x;

      save_scheduler_state(last_term);
      if(curr_term == get_cur_term()){
         set_vid_mem((uint32_t)0xB8000);
         memcpy(get_vid_mem(), terminals[curr_term].screen, 2 * NUM_ROWS * NUM_COLS);
         update_cursor(screen_y, screen_x);
         memcpy(keyboard_buf, terminals[curr_term].input_buf, MAX_BUF_SIZE);
      }


      asm volatile("       \n\
            movl %%ebp, %%eax    \n\
            movl %%esp, %%ebx    \n\
      "
      :"=a"(old_pcb->EBP_SWITCH), "=b"(old_pcb->ESP_SWITCH));

      asm volatile(""
           "mov %0, %%esp;"
           "mov %1, %%ebp;"
           //"jmp HALTED;"
           :                      /* no outputs */
           :"r"(new_pcb->ESP_SWITCH), "r"(new_pcb->EBP_SWITCH)   /* inputs */ 
      );
}

uint8_t get_curr_exec_term(){
   return curr_term;
}

void save_scheduler_state(uint8_t temp_cur_term){
   terminals[temp_cur_term].x = screen_x;
   terminals[temp_cur_term].y = screen_y;
   memcpy(terminals[temp_cur_term].screen, get_vid_mem(), 2 *NUM_ROWS * NUM_COLS);
   memcpy(terminals[temp_cur_term].input_buf, keyboard_buf, MAX_BUF_SIZE);
}

void restore_scheduler_state(int newt){
   memcpy(get_vid_mem(), terminals[newt].screen, 2 * NUM_ROWS * NUM_COLS);
   screen_x = terminals[newt].x;
   screen_y = terminals[newt].y;
   update_cursor(screen_y, screen_x);
   memcpy(keyboard_buf, terminals[newt].input_buf, MAX_BUF_SIZE);
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
