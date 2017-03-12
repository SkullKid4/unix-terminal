/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define INTS_PER_PIC 8
#define INT_MAX 16
#define SLAVE_IDX 2

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init()
{

 	 // Mask all interrupts
 	 master_mask = 0xff;
 	 outb(master_mask, MASTER_8259_PORT + 1);	//+1s for data port
 	 slave_mask = 0xff;
 	 outb(slave_mask, SLAVE_8259_PORT + 1);

 	 // Initialize master.
 	 outb (ICW1, MASTER_8259_PORT);
 	 outb (ICW2_MASTER ,MASTER_8259_PORT + 1);
 	 outb (ICW3_MASTER, MASTER_8259_PORT + 1);
 	 outb (ICW4, MASTER_8259_PORT + 1);

 	 // Initialize Slave
 	 outb (ICW1, SLAVE_8259_PORT);
 	 outb (ICW2_SLAVE, SLAVE_8259_PORT + 1);
 	 outb (ICW3_SLAVE, SLAVE_8259_PORT + 1); 
 	 outb (ICW4, SLAVE_8259_PORT + 1);

	 //Restore the mask
	 outb(master_mask, MASTER_8259_PORT + 1);
 	 outb(slave_mask, SLAVE_8259_PORT + 1);

}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	if (irq_num < INTS_PER_PIC) {
		master_mask &= ~(0x1 << irq_num);
		outb(master_mask, MASTER_8259_PORT + 1);
		//enable specified IRQ on master

	}
	else if (irq_num < INT_MAX) {
		//enable (irq_num - 8) on slave
		slave_mask &= ~(0x1 << (irq_num - INTS_PER_PIC));
		outb(slave_mask,SLAVE_8259_PORT + 1);
		enable_irq(SLAVE_IDX);

	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	if (irq_num < INTS_PER_PIC) {
		master_mask |= (0x1 << irq_num);
		outb(master_mask, MASTER_8259_PORT + 1);
		//disable specified IRQ on master
	}
	else if (irq_num < INT_MAX) {
		//disable (irq_num - 8) on slave
		slave_mask |= (0x1 << (irq_num - INTS_PER_PIC));
		outb(slave_mask,SLAVE_8259_PORT + 1);
		disable_irq(SLAVE_IDX);
	}
}


/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	uint8_t send_eoic;
	if (irq_num < INTS_PER_PIC){
		send_eoic = EOI | irq_num;
		outb(send_eoic,MASTER_8259_PORT);
	// output irq_num | EOI to the PIC
	}
	else if (irq_num < INT_MAX) {
		//output (iqr_num - 8) | EOI to slave pic
		send_eoic = EOI | (irq_num - INTS_PER_PIC);
		outb(send_eoic, SLAVE_8259_PORT );
		send_eoi(SLAVE_IDX);// also send this to master
	}
}
