#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"

volatile int rtc_interrupt;
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
  //test_interrupts();
  outb(0x0C, RTC_REGISTER_PORT); //select register C
  inb(RTC_DATA_PORT); // read from register C to enable more interrupts.
  rtc_interrupt = 1;
  sti();
}
int rtc_open(void) {
  set_rate(2);
  return 0;
}

int rtc_close(void) {
  return 0;
}

int rtc_read(void) {
  int x = 3;
  //rtc_interrupt = 0;
  //printf ("waiting for interrupt");
  while (!rtc_interrupt){
  }
  //printf("got interrupt\n");
  x++;
  rtc_interrupt = 0;
  //printf("cleared flag");
  return 0;
}
int rtc_write(int * buf, int nbytes) {
  if (nbytes != 4 || buf == NULL)
    return -1;
  int freq = *buf;
  return set_rate(freq);
}
int set_rate(int freq) {
  /* Save old value of Reg A*/
  int8_t freq_const;
  outb(REGISTER_A, RTC_REGISTER_PORT);
  int8_t a_old = inb(RTC_DATA_PORT);
  switch (freq) {
    case 1024:
      freq_const = HZ_1024; 
      break;
    case 512: 
      freq_const = HZ_512; 
      break;
    case 256: 
      freq_const = HZ_256; 
      break;
    case 128:
      freq_const = HZ_128; 
      break;
    case 64: 
      freq_const = HZ_64; 
      break;
    case 32: 
      freq_const = HZ_32; 
      break;
    case 16: 
      freq_const = HZ_16; 
      break;
    case 8: 
      freq_const = HZ_8; 
      break;
    case 4: 
      freq_const = HZ_4; 
      break;
    case 2: 
      freq_const = HZ_2; 
      break;
    case 0: 
      freq_const = HZ_0; 
      break;
    default:
      return -1;
    }
    outb(REGISTER_A, RTC_REGISTER_PORT);
    outb((0xF0 & a_old) | freq_const, RTC_DATA_PORT);
    return FREQ_SUCCESS; // Return 4 on success 
}

void test_rtc(void) {
  rtc_open();
  //printf("opened rtc");
  while (1){
    rtc_read();
    //printf("read rtc");
    printf("l");
  }
}
void toggle_freq(void) {
  int8_t freq_const;
  outb(REGISTER_A, RTC_REGISTER_PORT);
  int8_t a_old = inb(RTC_DATA_PORT);
  if (a_old != 0)
    freq_const = a_old - 1;
  else 
    freq_const = 0x0F;
  outb(REGISTER_A, RTC_REGISTER_PORT);
  outb((0xF0 & a_old) | freq_const, RTC_DATA_PORT);

}
