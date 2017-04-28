#include "lib.h"
#include "syscall_link.h"
#include "idt.h"
#include "i8259.h"
#include "pit.h"
#include "x86_desc.h"

volatile int32_t timer_ticks=0;
volatile int32_t pit_lock =0;
#define CHANNEL_0 0x40
#define CHANNEL_1 0x41
#define CHANNEL_2 0X42
#define PIT_IRQ   0
#define PIT_IDT_IDX 0x20
void pit_init (){
/* 	mov al, 0x36
out 0x43, al    ;tell the PIT which channel we're setting

mov ax, 11931	//100hz 1193182 / 100 = 11931
out 0x40, al    ;send low byte
mov al, ah
out 0x40, al    ;send high byte*/
	
	outb(0x36,0x43);
	outb(0x9B,CHANNEL_0);
	outb(0x2E,CHANNEL_0);
	SET_IDT_ENTRY(idt[PIT_IDT_IDX],(pit_linkage));
	enable_irq(PIT_IRQ);
}

void pit_handler()
{
    /* Increment our 'tick count' */
	if(pit_lock==0){
		pit_lock=1;
		cli();
		timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
		if (timer_ticks % 18 == 0)
		{
			puts("One second has passed\n");
		}
		sti();
		pit_lock=0;
	}
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
