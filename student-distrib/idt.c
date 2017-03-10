#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

#define NUM_EXCEPTIONS 32


/* sam playing with function, maybe other one is fine */
void idt_init()
{
	int i, j;
	for(i = 0; i < NUM_VEC; i++) {
		idt[i].present = 1;
		idt[i].dpl = 0;
		idt[i].reserved0 = 0;
		idt[i].size = 1;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 0;
		idt[i].reserved4 = 0;
		idt[i].seg_selector = KERNEL_CS;
		SET_IDT_ENTRY(idt[i], test_interrupts);
	}
	for (i = 0; i < NUM_EXCEPTIONS; i++){
		//change setting on exceptions
	}

	//change setting on system call;


	//set individual idt entries
	SET_IDT_ENTRY(idt[0x00], zero_exc);
	SET_IDT_ENTRY(idt[0x20], keyboard_interrupt)

	lidt(idt_desc_ptr);
}


// actually not needed given test interrupt
void gen_interrupt(void)
{
	printf("Something happened. We will handle what later.");
}


void zero_exc(void) 
{
	printf("Division by zero");
}

/*start of what alex wrote*/
void idt_init()
{
	int i;
	for(i = 0; i < NUM_EXCEPTIONS; i++) {
		SET_IDT_ENTRY(idt[i], handle_exception(i));
	}
}

void handle_exception(int n)
{
	switch(n) {
		case 0:
			puts("Division by zero");

		case 1:
			puts("Debugger");

		case 2:
			puts("NMI");

		case 3:
			puts("Breakpoint");

		case 4:
			puts("Overflow");

		case 5:
			puts("Bounds");

		case 6:
			puts("Invalid Opcode");

		case 7:
			puts("Coprocessor not availabe");

		case 8:
			puts("Double fault");

		case 9:
			puts("Coprocessor Segment Overrun");
				
		case 10:
			puts("Invalid Task State Segment");

		case 11:
			puts("Segment not present");

		case 12:
			puts("Stack Fault");

		case 13:
			puts("General protection fault");

		case 14:
			puts("Page fault");

		case 15:
			puts("reserved: do not use");

		case 16:
			puts("Math Fault");

		case 17:
			puts("Alignment Check");

		case 18:
			puts("Machine Check");

		case 19:
			puts("SIMD Floating-Point Exception");

		case 20:
			puts("Virtualization Exception");

		case 21:
			puts("Control Protection Exception");

		case 22:
			puts("reserved: do not use");

		case 23:
			puts("reserved: do not use");

		case 24:
			puts("reserved: do not use");

		case 25:
			puts("reserved: do not use");

		case 26:
			puts("reserved: do not use");

		case 27:
			puts("reserved: do not use");

		case 28:
			puts("reserved: do not use");

		case 29:
			puts("reserved: do not use");

		case 30:
			puts("reserved: do not use");

		case 31:
			puts("reserved: do not use");


	}
}