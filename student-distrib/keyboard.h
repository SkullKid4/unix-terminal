#ifndef KEYBOARD_H
#define KEYBOARD_H


#define KEYBOARD_IRQ			1		//this is the irq number on the master PIC
#define KEYBOARD_STATUS_PORT	0x64		//port for reading keyboard data
#define KEYBOARD_DATA_PORT		0x60
#define KEYBOARD_IDT_IDX		0x21

void keyboard_handler();
void keyboard_init();



#endif
