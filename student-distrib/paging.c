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
uint32_t user_video_page_table[NUM_PAGE_TABLES] __attribute__((aligned(FOUR_KB)));
uint32_t vidpagetable[NUM_PAGE_TABLES]  __attribute__((aligned(FOUR_KB)));

/*
void paging_init()
  Input: none
  Return Value: none
  Function: initializes the page directory and page table for memory from 0-8MB;
	sets 4-8MB to be the kernel, and enables video memory inside 0-4MB
*/
int paging_init(void) {
	int i;
	int video_page_table_offset;
	page_directory[0] = 0x03; // read/write and present
	page_directory[1] = 0x87; //kernel space; enable 4mb mode, supervisor mode, read/write, and present
	for (i = 2; i < NUM_PAGE_DIRECTORIES; i++) {
		page_directory[i] = 0x02; //initialize all other page directories to set read/write and
								  // not present
	}
	for (i = 0; i < NUM_PAGE_TABLES; i++) {
		page_table[i] = ((i * FOUR_KB))| 2; //initialize the page table for the first directory
											// enable read/write & set everything to not present.
	}
	// Find what entry in the page table corresponds to video memory.
	video_page_table_offset = (VIDEO / FOUR_KB); 
	page_table[video_page_table_offset] |= 1;
	page_directory[0] |= ((unsigned int)page_table);
	page_directory[1] |= (FOUR_KB * ONE_KB);

	/*enable paging*/
	asm (
	"movl $page_directory, %%eax  	  ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000010, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );


	return 0;

}

/*
void map
  Input: virt_add - virtual address
  		phys_add - physical address
  Return Value: none
  Function: maps a physical address to a virtual address, marking it as present
  			also flushes the TLB
*/
 void map(uint32_t virt_add, uint32_t phys_add) {
 		uint32_t pde = virt_add / FOUR_MB;
 		page_directory[pde] = phys_add | 0x87;
 		asm (
 	"movl	%%cr3,%%eax ;"
	"movl	%%eax,%%cr3 "
	: : :"eax", "cc");
 	}
/*
void map_w_pt
  Input: virt_add - virtual address
  		phys_add - physical address
  Return Value: none
  Function: maps a video memory physical address to a virtual address, marking it as present
  			also flushes the TLB
*/
 void map_w_pt(uint32_t virt_add, uint32_t phys_add) {
 	uint32_t pde = virt_add / FOUR_MB;
 	page_directory[pde] = ((uint32_t)user_video_page_table) | 0x7;
 	user_video_page_table[0] = phys_add | 0x7;
 	 asm (
 	"movl	%%cr3,%%eax ;"
	"movl	%%eax,%%cr3 "
	: : :"eax", "cc");
 }

 void map_video_w_pt(uint32_t virt_add, uint32_t phys_add) {
 	uint32_t pde = virt_add / FOUR_MB;
 	page_directory[pde] = ((uint32_t)user_video_page_table) | 0x7;
 	user_video_page_table[0] = phys_add | 0x7;
 	 asm (
 	"movl	%%cr3,%%eax ;"
	"movl	%%eax,%%cr3 "
	: : :"eax", "cc");
 }
