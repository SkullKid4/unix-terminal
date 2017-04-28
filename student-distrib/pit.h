#ifndef PIT_H
#define PIT_H

#define CHANNEL_0 0x40
#define CHANNEL_1 0x41
#define CHANNEL_2 0X42
#define PIT_IRQ   0
#define PIT_IDT_IDX 0x20
#define LOW_BYTE	0x9B
#define HIGH_BYTE	0x2E
#define PIT_PORT	0x36
#define SELECT_CHANNEL	0x43


void pit_init();
void pit_handler();

#endif
