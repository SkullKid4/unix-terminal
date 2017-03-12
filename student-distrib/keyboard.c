#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"

volatile unsigned lock = 0;       //used to lock the thread when writing keybored output to the screen

/*
this is a map I retrived from https://github.com/arjun024/mkeykernel/blob/master/keyboard_map.h
All it does is converts the raw output from the keyboared into the ascii chars they are associated with
*/
unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/*
keyboard_init
  DESCRIPTION: sets the 0-15, 16-31 bits to point to the keyboard handler we defined
  INPUT: none
  OUTPUT: none
*/
void keyboard_init(){
	SET_IDT_ENTRY(idt[KEYBOARD_IDT_IDX], (keyboard_handler));
}

/*
keyboard_handler
  DESCRIPTION: When a putton press or release occurs this function is called and the response is
               handled appropriatly
  INPUT: none
  OUTPUT: none 
*/
void keyboard_handler(){
	unsigned char status;    //used to check keyboard status
	int keycode;             //holds the raw output of the keyboard
  if(lock == 0){
    lock = 1;             //lock the thread and blocks intrrupts
    cli();

	  send_eoi(KEYBOARD_IRQ);

    status = inb(KEYBOARD_STATUS_PORT);
  
    if (status & 0x01) {                    //if the status is set, get the code from the keyboard port
      keycode = inb(KEYBOARD_DATA_PORT);
      if(keycode < 0 || keycode > 0x7F){    //if this is a button release code, unlock and turn on interrupts
        lock = 0;
        sti();
        return;
      }
      putc((char)keyboard_map[keycode]);    //write the value of the ascii char to the screen

    }
    sti();          //enable interrupts and unlock
    lock = 0;
  }
}
