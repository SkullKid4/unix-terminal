#ifndef PAGING_H
#define PAGING_H

#include "lib.h"

#define NUM_PAGE_DIRECTORIES 1024
#define NUM_PAGE_TABLES 1024
#define FOUR_KB	4096
#define ONE_KB 1024
#define FOUR_MB 0x400000

/*Initialize paging*/
int paging_init(void);


#endif
