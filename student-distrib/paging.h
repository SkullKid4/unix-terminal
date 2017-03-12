#ifndef PAGING_H
#define PAGING_H

#include "lib.h"

#define NUM_PAGE_DIRECTORIES 1024
#define NUM_PAGE_TABLES 1024
#define FOUR_KB	4096
#define ONE_KB 1024


int init_paging(void);

void enable_Paging(uint32_t page_directory1);

#endif
