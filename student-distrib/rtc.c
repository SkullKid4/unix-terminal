#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"

void rtc_init(){
  char prev;
  //outb(0x8A, RTC_REGISTER_PORT) // select Status Register A, and disable NMI
  //outb(0x20, RTC_DATA_PORT) // write to CMOS/RTC RAM

  outb(0x8B, RTC_REGISTER_PORT) // select Register B, disable NMI
  prev = inb(RTC_DATA_PORT); // get current value of B
  outb(0x8B, RTC_REGISTER_PORT) // select Register B again
  outb(prev | 0x40, RTC_DATA_PORT) // set bit 6 of Register B
	SET_IDT_ENTRY(idt[RTC_IDT_IDX], (rtc_handler));
}

void rtc_handler(){
  cli();
	/* write EOI */
	send_eoi(KEYBOARD_IRQ);
  test_interrupts();
  outb(0x0C, RTC_REGISTER_PORT); //select register C
  inb(RTC_DATA_PORT); // read from register C to enable more interrupts.
  sti();
}
