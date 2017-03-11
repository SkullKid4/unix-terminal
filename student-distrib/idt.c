#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

#define NUM_EXCEPTIONS 32


//array of exception strings to print
/*static char* exception_print[32] = {"Division by zero", "Debugger", "NMI", "Breakpoint", "Overflow", "Bounds", "Invalid Opcode", "Coprocessor not available", "Double fault", "Coprocessor Segment Overrun", "Invalid Task State Segment",
									"Segment not present", "Stack Fault", "General protection fault", "Page fault", "Reserved by Intel, do no use", "Math Fault", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception", "Virtualization Exception",
									"Control Protection Exception", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use",}*/



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
	SET_IDT_ENTRY(idt[0], exc_0);
	SET_IDT_ENTRY(idt[1], exc_1);
	SET_IDT_ENTRY(idt[2], exc_2);
	SET_IDT_ENTRY(idt[3], exc_3);
	SET_IDT_ENTRY(idt[4], exc_4);
	SET_IDT_ENTRY(idt[5], exc_5);
	SET_IDT_ENTRY(idt[6], exc_6);
	SET_IDT_ENTRY(idt[7], exc_7);
	SET_IDT_ENTRY(idt[8], exc_8);
	SET_IDT_ENTRY(idt[9], exc_9);
	SET_IDT_ENTRY(idt[10], exc_10);
	SET_IDT_ENTRY(idt[11], exc_11);
	SET_IDT_ENTRY(idt[12], exc_12);
	SET_IDT_ENTRY(idt[13], exc_13);
	SET_IDT_ENTRY(idt[14], exc_14);
	SET_IDT_ENTRY(idt[15], exc_15_22_31);
	SET_IDT_ENTRY(idt[16], exc_16);
	SET_IDT_ENTRY(idt[17], exc_17);
	SET_IDT_ENTRY(idt[18], exc_18);
	SET_IDT_ENTRY(idt[19], exc_19);
	SET_IDT_ENTRY(idt[20], exc_20);
	SET_IDT_ENTRY(idt[21], exc_21);
	for(i = 22; i < 32; i++) {
		SET_IDT_ENTRY(idt[i], exc_15_22_31);
	}

	//change setting on system call;


	//set individual idt entries
	//SET_IDT_ENTRY(idt[0x00], zero_exc);
	//SET_IDT_ENTRY(idt[0x20], keyboard_interrupt);

	lidt(idt_desc_ptr);
	puts("Finished enabling interrupts");
}

/*void handle_exception(int exc)
{
	printf(*exception_print);
}*/


// actually not needed given test interrupt
void gen_interrupt(void)
{
	printf("Something happened. We will handle what later.");
}


void exc_0() 
{
	puts("Division by zero");
}

void exc_1()
{
	puts("Debugger");
}

void exc_2()
{
	puts("NMI");
}

void exc_3()
{
	puts("Breakpoint");
}

void exc_4()
{
	puts("Overflow");
}

void exc_5()
{
	puts("Bounds");
}

void exc_6()
{
	puts("Invalid Opcode");
}

void exc_7()
{
	puts("Coprocessor not available");
}

void exc_8()
{
	puts("Double fault");
}

void exc_9()
{
	puts("Coprocessor Segment Overrun");
}

void exc_10()
{
	puts("Invalid Task State Segment");
}

void exc_11()
{
	puts("Segment not present");
}

void exc_12()
{
	puts("Stack Fault");
}

void exc_13()
{
	puts("General protection fault");
}

void exc_14()
{
	puts("Page fault");
}

void exc_15_22_31()	//this one corresponds to 15 and 22-31
{
	puts("Reserved by Intel: do not use");
}

void exc_16()
{
	puts("Math Fault");
}

void exc_17()
{
	puts("Alignment Check");
}

void exc_18()
{
	puts("Machine Check");
}

void exc_19()
{
	puts("SIMD Floating-Point Exception");
}

void exc_20()
{
	puts("Virtualization Exception");
}

void exc_21()
{
	puts("Control Protection Exception");
}

/*start of what alex wrote*/
/*void idt_init()
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
}*/
