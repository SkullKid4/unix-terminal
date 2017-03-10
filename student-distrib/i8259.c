/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_i
{
	/*I found this block of code when I was searching for something else. I'm going to look through it and adapt it - Sam.

	/*
	  #define PIC0_CTRL 0x20    // Master PIC control register address. 
 	  #define PIC0_DATA 0x21   // Master PIC data register address.

 	 // Mask all interrupts
 	 outb (PIC0_DATA, 0xff);

 	 // Initialize master.
 	 outb (PIC0_CTRL, 0x11); // ICW1: single mode, edge triggered, expect ICW4. /
 	 outb (PIC0_DATA, 0x20); // ICW2: line IR0...7 -> irq 0x20...0x27. /
 	 outb (PIC0_DATA, 0x04); // ICW3: slave PIC on line IR2. /
 	 outb (PIC0_DATA, 0x01); // ICW4: 8086 mode, normal EOI, non-buffered. 

  	/* Unmask all interrupts. 
 	 outb (PIC0_DATA, 0x00);
 	 */

}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	if (irq_num < 8) {
		//enable specified IRQ on master
	}
	else if (irq_num < 16) {
		//enable (irq_num - 8) on slave
		enable_irq(2);

	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	if (irq_num < 8) {
		//disable specified IRQ on master
	}
	else if (irq_num < 16) {
		//disable (irq_num - 8) on slave
		disable_irq(2);

}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if (irq_num < 8){
	// output irq_num | EOI to the PIC
	}
	else if (irq_num < 16) {
		//output (iqr_num - 8) | EOI to slave pic
		send_eoi(2);// also send this to master
	}
}

