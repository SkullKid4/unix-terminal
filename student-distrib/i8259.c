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
i8259_init()
{


	
	  #define PIC0_CTRL 0x20    // Master PIC control register address. 
 	  #define PIC0_DATA 0x21   // Master PIC data register address.

 	 // Mask all interrupts
 	 master_mask = 0xff;
 	 outb(MASTER_8259_PORT + 1, master_mask);
 	 slave_mask = 0xff;
 	 outb(SLAVE_8259_PORT + 1, slave_mask);

 	 // Initialize master.
 	 outb (MASTER_8259_PORT, ICW1); 
 	 outb (MASTER_8259_PORT + 1, ICW2_MASTER); 
 	 outb (MASTER_8259_PORT + 1, ICW3_MASTER);
 	 outb (MASTER_8259_PORT + 1, ICW4); 

 	 // Initialize Slave
 	 outb (SLAVE_8259_PORT, ICW1);
 	 outb (SLAVE_8259_PORT + 1, ICW2_SLAVE); 
 	 outb (SLAVE_8259_PORT + 1, ICW3_SLAVE); 
 	 outb (SLAVE_8259_PORT + 1, ICW4); 


}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	if (irq_num < 8) {
		master_mask &= ~(0x1 << irq_num);
		outb(MASTER_8259_PORT + 1, master_mask);
		//enable specified IRQ on master

	}
	else if (irq_num < 16) {
		//enable (irq_num - 8) on slave
		slave_mask &= ~(0x1 << (irq_num - 8));
		outb(SLAVE_8259_PORT + 1, slave_mask);
		enable_irq(2);

	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	if (irq_num < 8) {
		master_mask |= (0x1 << irq_num);
		outb(MASTER_8259_PORT + 1, master_mask);
		//disable specified IRQ on master
	}
	else if (irq_num < 16) {
		//disable (irq_num - 8) on slave
		slave_mask |= (0x1 << (irq_num - 8));
		outb(SLAVE_8259_PORT + 1, slave_mask);
		disable_irq(2);
	}
}


/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	uint8_t send_eoic;
	if (irq_num < 8){
		send_eoic = EOI | irq_num;
		outb(MASTER_8259_PORT, send_eoic);
	// output irq_num | EOI to the PIC
	}
	else if (irq_num < 16) {
		//output (iqr_num - 8) | EOI to slave pic
		send_eoic = EOI | (irq_num - 8);
		outb(MASTER_8259_PORT, send_eoic);
		send_eoi(2);// also send this to master
	}
}

