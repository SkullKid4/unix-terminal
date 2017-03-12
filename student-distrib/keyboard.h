#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEYBOARD_IRQ			1		//this is the irq number on the master PIC
#define KEYBOARD_STATUS_PORT	0x64		//port for reading keyboard stsatus
#define KEYBOARD_DATA_PORT		0x60		//port for reading keybored data
#define KEYBOARD_IDT_IDX		0x21		//the keyboreds index into the idt

/* function that is called from the idt table when interrupts occor*/
void keyboard_handler();

/*sets the idt entry with the handdlers adrs*/
void keyboard_init();



#endif
