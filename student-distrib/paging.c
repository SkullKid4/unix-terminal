#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "i8259.h"



#define VIDEO 0xB8000
#define NUM_COLS 80
#define NUM_ROWS 25
#define BYTES_PER_LOCATION 2
#define VIDEO_BYTES NUM_COLS * NUM_ROWS * BYTES_PER_LOCATION
#define ATTRIB 0x7

uint32_t page_directory[NUM_PAGE_DIRECTORIES] __attribute__((aligned(FOUR_KB)));
uint32_t page_table[NUM_PAGE_TABLES]  __attribute__((aligned(FOUR_KB)));


void enable_Paging(uint32_t page_directory1){ 
	asm volatile("                  \n\
		movl 8(%ebp), %eax		\n\
		movl %eax, %cr3			\n\
		movl %cr4, %ecx			\n\
		orl 0x10, %ecx				\n\
		movl %ecx, %cr4			\n\
		movl %cr0, %ecx			\n\
		orl 0x80000000, %ecx		\n\
		movl %ecx, %cr0			\n\
		"
		);
}

void init_paging(void) {
	int i;
	int video_page_table_offset;
	page_directory[0] = 0x01;
	page_directory[1] = 0x81; //kernel space
	for (i = 2; i < NUM_PAGE_DIRECTORIES; i++) {
		page_directory[i] = 0x00;
	}
	for (i = 0; i < NUM_PAGE_TABLES; i++) {
		page_table[i] = (i * 0x1000) | 0;
	}
	video_page_table_offset = (VIDEO / FOUR_KB) - 1;
	page_table[video_page_table_offset] |= 1;

	page_directory[0] |= ((unsigned int)page_table);

	page_directory[1] |= (FOUR_KB * 1024) << 12;
	enable_Paging((uint32_t)page_directory);

}
