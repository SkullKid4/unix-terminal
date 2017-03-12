#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "syscall.h"

#define NUM_EXCEPTIONS 32


//array of exception strings to print
/*static char* exception_print[32] = {"Division by zero", "Debugger", "NMI", "Breakpoint", "Overflow", "Bounds", "Invalid Opcode", "Coprocessor not available", "Double fault", "Coprocessor Segment Overrun", "Invalid Task State Segment",
									"Segment not present", "Stack Fault", "General protection fault", "Page fault", "Reserved by Intel, do no use", "Math Fault", "Alignment Check", "Machine Check", "SIMD Floating-Point Exception", "Virtualization Exception",
									"Control Protection Exception", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use", "Reserved by Intel, do no use",}*/




void idt_init()
{
	int i;
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
		SET_IDT_ENTRY(idt[i], gen_interrupt);
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



	//change setting on system call
	SET_IDT_ENTRY(idt[0x80], system_call_handler);

	//lidt(idt_desc_ptr);
	puts("Finished enabling interrupts");
}

/*void handle_exception(int exc)
{
	printf(*exception_print);
}*/


// Generic Interrupt. If a not defined interrupt is called, this outputs.
void gen_interrupt(void)
{
	clear();
	printf("Something happened. We will handle what later.");
	while(1);
}

// Division by zero exception. prints error and spins
void exc_0() 
{
	clear();
	puts("Division by zero");
	while(1);
}

// Debugging Exception. prints error and spins
void exc_1()
{	
	clear();
	puts("Debugger");
	while(1);
}

// NMI Exception. prints error and spins
void exc_2()
{	
	clear();
	puts("NMI");
	while(1);
}


// Breakpoint Exception. prints error and spins
void exc_3()
{
	clear();
	puts("Breakpoint");
	while(1);
}

// Overflow Exception. prints error and spins
void exc_4()
{	clear();
	puts("Overflow");
	while(1);
}

// Bounds Exception. prints error and spins
void exc_5()
{	clear();
	puts("Bounds");
	while(1);
}

// Invalid Opcode Exception. prints error and spins
void exc_6()
{	clear();
	puts("Invalid Opcode");
	while(1);
}


// Coprocessor not available exception. prints error and spins
void exc_7()
{	clear();
	puts("Coprocessor not available");
	while(1);
}

// Double Fault Exception. prints error and spins
void exc_8()
{	clear();
	puts("Double fault");
	while(1);
}

// Coprocessor Segment Overrun Exception. prints error and spins
void exc_9()
{	clear();
	puts("Coprocessor Segment Overrun");
	while(1);
}

// Invalid Task State Segment Exception. prints error and spins
void exc_10()
{	clear();
	puts("Invalid Task State Segment");
	while(1);
}

// Segment not Present Exception. prints error and spins
void exc_11()
{	clear();
	puts("Segment not present");
	while(1);
}

// Stack Fault Exception. prints error and spins
void exc_12()
{	clear();
	puts("Stack Fault");
	while(1);
}

//General Protection Fault Exception. prints error and spins
void exc_13()
{	clear();
	puts("General protection fault\n"
		"\n"
		"\n"
		
		"   R T D C       R T D C R T D C    R T D C        R T D C R    \n"
		"   T      R             T           T      R       T            \n" 
		"   D       T            D           D       T      D            \n" 
		"   C      D             C           C        D     C            \n" 
		"   R T D C              R           R        C     R            \n" 
		"   T      R             T           T        R     T            \n" 
		"   D       T            D           D       T      D            \n" 
	 	"   C        C           C           C R T D        C D T R C    \n"

);
	while(1);
}

// Page Fault Exception. prints error and spins
void exc_14()
{	clear();
	puts("Page fault");
	while(1);
}

// Reserved Exception. prints error and spins
void exc_15_22_31()	//this one corresponds to 15 and 22-31
{	clear();
	puts("Reserved by Intel: do not use");
	while(1);
}

// Math Fault Exception
void exc_16()
{	clear();
	puts("Math Fault");
	while(1);
}

// Alignment Check Exception; prints error and spins
void exc_17()
{	clear();
	puts("Alignment Check");
	while(1);
}

// Machine Check Exception. prints error and spins
void exc_18()
{	clear();
	puts("Machine Check");
	while(1);
}

// SIMD Floating-Point Exception. prints error and spins
void exc_19()
{	clear();
	puts("SIMD Floating-Point Exception");
	while(1);
}
// Virtualization Exception. prints error and spins
void exc_20()
{	clear();
	puts("Virtualization Exception");
	while(1);
}

// Control Protection Exception. prints error and spins
void exc_21()
{	clear();
	puts("Control Protection Exception");
	while(1);
}

