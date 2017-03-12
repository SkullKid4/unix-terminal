#ifndef RTC_H
#define RTC_H


#define RTC_IRQ			8		//this is the irq number on the master PIC
#define RTC_REGISTER_PORT	0x70	
#define RTC_DATA_PORT	0x71
#define RTC_IDT_IDX		0x28

/*Initialize RTC interrupts*/
void rtc_handler();
void rtc_init();



#endif
