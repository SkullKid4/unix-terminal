#ifndef SCHEDULER
#define SCHEDULER

#include "types.h"
#include "lib.h"

// typedef struct registers_struct
// {
// 	int32_t edi;
// 	int32_t esi;
// 	int32_t user_esp;
// 	int32_t ebp;
// 	int32_t esp;
// 	int32_t ebx;
// 	int32_t edx;
// 	int32_t ecx;
// 	int32_t eax;
// } registers_t;

typedef struct process_struct
{
    //registers_t regs;
    //int32_t *cr3;
    int active;
    int ESP0;
    int PID;
    //struct process_struct *next;
} process_t;

volatile process_t task_array[3];

void switch_task();
void save_scheduler_state(uint8_t temp_cur_term);
void restore_scheduler_state(int newt);
extern void scheduler_init();
extern void add_task(int esp0, int pid);
extern void remove_task(int pid);
<<<<<<< HEAD
=======
uint8_t get_curr_exec_term();
void set_curr_exec_term(int term);
>>>>>>> samalexversion

#endif

