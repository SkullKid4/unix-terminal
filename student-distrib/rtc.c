#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"
#include "syscall.h"

volatile int rtc_interrupt;
volatile unsigned rtc_lock = 0;
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
  enable_irq(RTC_IRQ);
}

/*
rtc_handler()
  Input: none
  Return Value: none
  Function: call test interrupts when RTC is turned on. Select register C
  to make sure interrupts will happen again.
*/
void rtc_handler(){
  if (rtc_lock == 0) {
    rtc_lock =1;
    cli();
  /* write EOI */
    send_eoi(RTC_IRQ);
  //test_interrupts();
    outb(0x0C, RTC_REGISTER_PORT); //select register C
    inb(RTC_DATA_PORT); // read from register C to enable more interrupts.
    rtc_interrupt = 1; // set interrupt flag
    sti();
    rtc_lock = 0;
  }
}

/*
int32_t rtc_open
  Input: none
  Return Value: none
  Function: Open rtc sets the rate of the rtc to 2 hertz
*/
int32_t rtc_open(void) {
  while (rtc_lock == 1)
  {
  }
  rtc_lock = 1;
  (void)set_rate(INITIAL_RATE);
  rtc_lock = 0;
  return 0;
}

// rtc_close. Just returns 0.
/*
int32_t rtc_close
  Input: none
  Return Value: 0
  Function: RTC close
*/
int32_t rtc_close(void) {
  return 0;
}

/*
int32_t rtc_read
  Input: none
  Return Value: 0
  Function: waits for the next rtc interrupt.
	spins until that interrupt occurs
*/
int32_t rtc_read(void) {
  int32_t x = 3;
  while (!rtc_interrupt){
  }
  x++;
  rtc_interrupt = 0;
  return 0;
}

/*
int32_t rtc_write
  Input: buf -- frequency to write to 
		 nbytes -- number of bytes to write to
  Return Value: 4 -- success -1 -- failure
  Function: write to RTC with frequeny rate
*/
int rtc_write(int32_t * buf, int32_t nbytes) {
  cli();
  if (nbytes != 4 || buf == NULL)
    return -1;
  int32_t freq = *buf;
  return set_rate(freq);
  sti();
}


/*
int32_t set_rate
  Input: freq -- frequency to set rate to 
  Return Value: 4 -- success 
  Function: Sets the rate of the RTC. Only powers of 2 Hertz of to 1024 Hz are valid
			if an invalid rate is requested, no change is made.
*/

int32_t set_rate(int32_t freq) {
  /* Save old value of Reg A*/
  int8_t freq_const;
  outb(REGISTER_A, RTC_REGISTER_PORT);
  int8_t a_old = inb(RTC_DATA_PORT);
  // change frequency constant
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
      return FREQ_SUCCESS;
    }
    // output frequency constant to register
    outb(REGISTER_A, RTC_REGISTER_PORT);
    outb((0xF0 & a_old) | freq_const, RTC_DATA_PORT);
    return FREQ_SUCCESS; // Return 4 on success 
}

/*
  void test_rtc
  Input: none
  Return Value: none 
  Function:  Function for testing the rtc system calls
			it open the rtc, setting the initial frequence to 0 HZ.
			Then, it prints the letter 'l' every time the RTC sends an interrupt.
*/

void test_rtc(void) {
  cli();
  rtc_open();
  int rtc_write_var[1];
  rtc_write_var[0] = 0;
  rtc_write(rtc_write_var, WRITE_BYTES);
  sti();
  char buf[1];
  buf[0] = 'l';
  while (1){
    rtc_read();
    write(STDOUT, buf, 1);
  }
}

/*
  void test_rtc
  Input: none
  Return Value: none 
  Function: Changes the frequency of the rtc. every time this is called, the frequency doubles,
			except at 1024 hz, it goes to 0, and at 0 hz, it goes to 2 hz
*/
void toggle_freq(void) {
  int8_t freq_const;
  outb(REGISTER_A, RTC_REGISTER_PORT);
  int8_t a_old = inb(RTC_DATA_PORT);
  // Change old frequency value
  if (a_old != 0)
    freq_const = a_old - 1;
  else 
    freq_const = 0x0F;
  // output the new frequence constant to 
  outb(REGISTER_A, RTC_REGISTER_PORT);
  outb((0xF0 & a_old) | freq_const, RTC_DATA_PORT);

}
