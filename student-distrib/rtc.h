#ifndef RTC_H
#define RTC_H


#define RTC_IRQ				8		//this is the irq number on the master PIC
#define RTC_REGISTER_PORT	0x70	
#define RTC_DATA_PORT		0x71
#define RTC_IDT_IDX			0x28
#define REGISTER_A			0x8A
#define FREQ_SUCCESS		4

//Set register A to these values for these frequencies
#define HZ_0			0x00
#define HZ_2			0x0F
#define HZ_4			0x0E
#define HZ_8			0x0D
#define HZ_16			0x0C
#define HZ_32			0x0B
#define HZ_64			0x0A
#define HZ_128			0x09
#define HZ_256			0x08
#define HZ_512			0x07
#define HZ_1024			0x06

//RTC-open sets the freqency to 2 hertz
#define INITIAL_RATE    2

//rtc-write accepts four byte arguments
#define WRITE_BYTES		4

/*InitialiZ_e RTC interrupts*/
void rtc_handler();
void rtc_init();
int rtc_open(void);
int rtc_close(void);
int rtc_read(void);
int rtc_write(int * buf, int nbytes);
int set_rate(int freq);
void test_rtc(void);
void toggle_freq(void);


#endif
