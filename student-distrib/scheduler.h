#ifndef SCHEDULER
#define SCHEDULER

#include "types.h"
#include "lib.h"

typedef struct registers_struct
{
	int32_t edi;
	int32_t esi;
	int32_t user_esp;
	int32_t ebp;
	int32_t esp;
	int32_t ebx;
	int32_t edx;
	int32_t ecx;
	int32_t eax;
} registers_t;

typedef struct process_struct
{
    registers_t regs;
    int32_t *cr3;
    struct process_struct *next;
} process_t;

process_t *pqueue;
process_t *current_proc;

void switch_task();

#endif

