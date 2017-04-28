#ifdef PIT_H
#define PIT_H

#define CHANNEL_0 0x40
#define CHANNEL_1 0x41
#define CHANNEL_2 0X42
#define PIT_IRQ   0
#define PIT_IDT_IDX 0x20

void pit_init();
void pit_handler();

#endif
