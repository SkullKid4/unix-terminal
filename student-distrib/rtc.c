#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
/*
rtc_init()
  Input: none
  Return Value: none
  Function: Turning on IRQ8 initialize RTC
*/
void rtc_init(){
  char prev;
  outb(0x8B, RTC_REGISTER_PORT); // select Register B, disable NMI
  prev = inb(RTC_DATA_PORT); // get current value of B
  outb(0x8B, RTC_REGISTER_PORT); // select Register B again
  outb(prev | 0x40, RTC_DATA_PORT); // set bit 6 of Register B
	SET_IDT_ENTRY(idt[RTC_IDT_IDX], (rtc_handler));
}

/*
rtc_handler()
  Input: none
  Return Value: none
  Function: call test interrupts when RTC is turned on. Select register C
  to make sure interrupts will happen again.
*/
void rtc_handler(){
  cli();
	/* write EOI */
	send_eoi(RTC_IRQ);
  test_interrupts();
  outb(0x0C, RTC_REGISTER_PORT); //select register C
  inb(RTC_DATA_PORT); // read from register C to enable more interrupts.
  sti();
}
